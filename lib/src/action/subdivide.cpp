#include <unordered_map>
#include <glm/glm.hpp>
#include "action/subdivide.hpp"
#include "action/unit/on.hpp"
#include "adjacent-iterator.hpp"
#include "partial-action/insert-edge-vertex.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "partial-action/triangulate-6-gon.hpp"
#include "partial-action/triangulate-quad.hpp"
#include "primitive/ray.hpp"
#include "primitive/triangle.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/face-intersection.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {
  typedef std::unordered_set <WingedFace*> FaceSet;

  struct SubdivideData {
          WingedMesh&                       mesh;
    const std::unordered_set <WingedFace*>& selection;
          std::unordered_set <WingedFace*>& affectedFaces;

    SubdivideData (WingedMesh& m, const std::unordered_set <WingedFace*>& s
                                ,       std::unordered_set <WingedFace*>& a) 
      : mesh           (m)
      , selection      (s)
      , affectedFaces  (a)
    {}
  };
};

struct ActionSubdivide::Impl {
  ActionUnitOn <WingedMesh> actions;

  void runUndo (WingedMesh& mesh) { this->actions.undo (mesh); }
  void runRedo (WingedMesh& mesh) { this->actions.redo (mesh); }

  void run ( WingedMesh& mesh, const std::unordered_set <WingedFace*>& selection
                             ,       std::unordered_set <WingedFace*>& affected ) 
  {
    this->subdivide (SubdivideData (mesh, selection, affected));
  }

  void subdivide (const SubdivideData& data) {
    FaceSet neighbourhood, border;
    neighbourhood.insert (data.selection.begin (), data.selection.end ());

    this->addOneRing           (neighbourhood);
    this->addOneRing           (neighbourhood);
    this->extendNeighbourhood  (neighbourhood);
    this->oneRingBorder        (neighbourhood, border);
    this->subdivideFaces       (data, neighbourhood);
    this->refineBorder         (data, border);
    this->smoothAffectedFaces  (data);
  }

  void addOneRing (FaceSet& neighbourhood) {
    FaceSet oneRing;
    for (WingedFace* n : neighbourhood) {
      for (WingedFace& face : n->adjacentFaces ()) {
        if (neighbourhood.count (&face) == 0) {
          oneRing.insert (&face);
        }
      }
    }
    neighbourhood.insert (oneRing.begin (), oneRing.end ());
  }

  void extendNeighbourhood (FaceSet& neighbourhood) {
    FaceSet extendedNeighbourhood;

    auto isInNeighbourhood = [&] (WingedFace& face) -> bool {
      return neighbourhood.count (&face) > 0 || extendedNeighbourhood.count (&face) > 0;
    };

    auto hasAtLeast2Neighbours = [&] (WingedFace& face) -> bool {
      unsigned int numNeighbours = 0;

      for (WingedFace& a : face.adjacentFaces ()) {
        if (isInNeighbourhood (a)) {
          numNeighbours++;
        }
      }
      return numNeighbours >= 2;
    };

    auto hasPoleVertex = [] (WingedFace& face) -> bool {
      for (WingedVertex& v : face.adjacentVertices ()) {
        if (v.valence () > 9) {
          return true;
        }
      }
      return false;
    };

    auto extendTo = [&] (WingedFace& face) -> bool {
      return hasAtLeast2Neighbours (face) || hasPoleVertex (face);
    };

    // checks adjacent faces of a neighbour
    std::function <void (WingedFace&)> checkAdjacents = [&] (WingedFace& neighbour) -> void {
      for (WingedFace& face : neighbour.adjacentFaces ()) {
        if (isInNeighbourhood (face) == false && extendTo (face)) {
          extendedNeighbourhood.insert (&face);
          checkAdjacents (face);
        }
      }
    };

    for (WingedFace* f : neighbourhood) {
      checkAdjacents (*f);
    }
    neighbourhood.insert (extendedNeighbourhood.begin (), extendedNeighbourhood.end ());
  }

  void oneRingBorder (const FaceSet& neighbourhood, FaceSet& border) {
    border.clear ();
    for (WingedFace* n : neighbourhood) {
      for (WingedFace& f : n->adjacentFaces ()) {
        if (neighbourhood.count (&f) == 0) {
          border.insert (&f);
        }
      }
    }
  }

  void subdivideFaces (const SubdivideData& data, const FaceSet& faces) {
    std::unordered_set<Id> subdividedEdges;

    auto subdivideEdge = [&] (WingedEdge& edge) -> void {
      if (subdividedEdges.count (edge.id ()) == 0) {
        WingedEdge& newEdge = this->actions.add <PAInsertEdgeVertex> ().run 
          (data.mesh, edge, edge.middle (data.mesh));

        subdividedEdges.insert (edge   .id ());
        subdividedEdges.insert (newEdge.id ());
      }
    };

    for (WingedFace* face : faces) {
      for (WingedEdge* edge : face->adjacentEdges ().collect ()) {
        subdivideEdge (*edge);
      }
      this->actions.add <PATriangulate6Gon> ().run (data.mesh, *face, &data.affectedFaces);
    }
  }

  void refineBorder (const SubdivideData& data, const FaceSet& border) {
    for (WingedFace* face : border) {
      this->actions.add <PATriangulateQuad> ()
                   .run (data.mesh, *face, &data.affectedFaces);
    }
  }

  void smoothAffectedFaces (const SubdivideData& data) {
    std::unordered_map <WingedVertex*,glm::vec3> positions;
    std::unordered_map <WingedVertex*,glm::vec3> deltas;

    // initialize positions
    for (WingedFace* face : data.affectedFaces) {
      assert (face->isTriangle ());
      for (WingedVertex& v : face->adjacentVertices ()) {
        if (positions.count (&v) == 0) {
          positions.emplace (&v, v.vector (data.mesh));
        }
      }
    }
    // compute deltas
    for (WingedFace* face : data.affectedFaces) {
      for (WingedVertex& v : face->adjacentVertices ()) {
        if (deltas.count (&v) == 0) {
          const glm::vec3    p       (positions.find (&v)->second);
                glm::vec3    delta   (0.0f);
                unsigned int valence (0);

          for (WingedEdge& e : v.adjacentEdges ()) {
            WingedVertex&   otherV  = e.otherVertexRef (v);
            auto            otherIt = positions.find (&otherV);
            const glm::vec3 otherP  = otherIt == positions.end ()
                                    ? otherV.vector (data.mesh)
                                    : otherIt->second;
            delta += otherP - p;
            valence++;
          }
          deltas.emplace (&v, delta / float (valence));
        }
      }
    }
    // update positions
    for (auto deltaIt : deltas) {
      auto posIt = positions.find (deltaIt.first);
      assert (posIt != positions.end ());

      const glm::vec3 newPos = posIt->second + deltaIt.second;
      positions.erase (posIt);
      positions.emplace (deltaIt.first, newPos);
    }
    // displace in normal direction
    for (auto posIt : positions) {
      WingedVertex& v         (*posIt.first);
      glm::vec3     newNormal (0.0f);
      unsigned int  valence   (0);

      for (WingedFace& f : v.adjacentFaces ()) {
        newNormal += f.triangle (data.mesh).normal ();
        valence++;
      }
      newNormal *= 1.0f / float (valence);

      WingedFaceIntersection intersection;
      const glm::vec3& newPos = data.mesh.intersects (PrimRay (posIt.second, newNormal), intersection)
                              ? intersection.position ()
                              : posIt.second;

      this->actions.add <PAModifyWVertex> ().move (data.mesh, *posIt.first, newPos);
    }
  }

  /*
  void relaxByEdgeFlip (FaceSet& faces, int threshold) {
    for (WingedFace* f : faces) {
      for (WingedEdge* e : f->adjacentEdges ().collect ()) {
        this->relaxByEdgeFlip (*e, threshold);
      }
    }
  }

  void relaxByEdgeFlip (const SubdivideData& data, int threshold) {
    for (const Id& id : *data.affectedFaces) {
      WingedFace& f = data.mesh.faceRef (id);

      for (WingedEdge* e : f.adjacentEdges ().collect ()) {
        this->relaxByEdgeFlip (*e, threshold);
      }

    }
  }

  void relaxByEdgeFlip (WingedEdge& edge, int threshold) {
    if (this->relaxiationIndex (edge) >= threshold) {
      actions.add <PAFlipEdge> ().run (edge);

      this->relaxByEdgeFlip (edge.leftPredecessorRef  (), threshold);
      this->relaxByEdgeFlip (edge.leftSuccessorRef    (), threshold);
      this->relaxByEdgeFlip (edge.rightPredecessorRef (), threshold);
      this->relaxByEdgeFlip (edge.rightSuccessorRef   (), threshold);
    }
  }

  int relaxiationIndex (const WingedEdge& edge) const {
    assert (edge.leftFaceRef  ().numEdges () == 3);
    assert (edge.rightFaceRef ().numEdges () == 3);

    return int (edge.vertex1Ref ().valence ())
         + int (edge.vertex2Ref ().valence ())
         - int (edge.vertexRef (edge.leftFaceRef  (), 2).valence ())
         - int (edge.vertexRef (edge.rightFaceRef (), 2).valence ());
  }
  */
};

DELEGATE_BIG3 (ActionSubdivide)
DELEGATE3     (void, ActionSubdivide, run, WingedMesh&, const std::unordered_set<WingedFace*>&, std::unordered_set<WingedFace*>&)
DELEGATE1     (void, ActionSubdivide, runUndo, WingedMesh&)
DELEGATE1     (void, ActionSubdivide, runRedo, WingedMesh&)
