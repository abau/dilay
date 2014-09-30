#include <unordered_map>
#include <glm/glm.hpp>
#include "action/subdivide.hpp"
#include "action/unit/on.hpp"
#include "adjacent-iterator.hpp"
#include "partial-action/flip-edge.hpp"
#include "partial-action/insert-edge-vertex.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "partial-action/triangulate-5-gon.hpp"
#include "partial-action/triangulate-6-gon.hpp"
#include "partial-action/triangulate-quad.hpp"
#include "primitive/ray.hpp"
#include "primitive/triangle.hpp"
#include "subdivision-butterfly.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/face-intersection.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {
  typedef std::unordered_set <WingedFace*> FaceSet;
  typedef std::unordered_set <WingedEdge*> EdgeSet;

  struct SubdivideData {
          WingedMesh& mesh;
    const EdgeSet&    selection;
          FaceSet&    affectedFaces;

    SubdivideData (WingedMesh& m, const EdgeSet& s, FaceSet& a) 
      : mesh          (m)
      , selection     (s)
      , affectedFaces (a)
    {}
  };
};

struct ActionSubdivide::Impl {
  ActionUnitOn <WingedMesh> actions;

  void runUndo (WingedMesh& mesh) { this->actions.undo (mesh); }
  void runRedo (WingedMesh& mesh) { this->actions.redo (mesh); }

  static void extendDomain (std::vector <WingedFace*>& domain) {
    std::unordered_set <WingedFace*> domainSet (domain.begin (), domain.end ());
    domain.clear ();

    Impl::addOneRing            (domainSet);
    Impl::addOneRing            (domainSet);
    Impl::extendToNeighbourhood (domainSet);

    domain.insert (domain.end (), domainSet.begin (), domainSet.end ());
  }

  static void addOneRing (FaceSet& domain) {
    FaceSet oneRing;
    for (WingedFace* d : domain) {
      for (WingedFace& f : d->adjacentFaces ()) {
        if (domain.count (&f) == 0) {
          oneRing.insert (&f);
        }
      }
    }
    domain.insert (oneRing.begin (), oneRing.end ());
  }

  static void extendToNeighbourhood (FaceSet& domain) {
    FaceSet extendedDomain;

    auto isInDomain = [&] (WingedFace& face) -> bool {
      return domain.count (&face) > 0 || extendedDomain.count (&face) > 0;
    };

    auto hasAtLeast2NeighboursInDomain = [&] (WingedFace& face) -> bool {
      unsigned int numInDomain = 0;

      for (WingedFace& a : face.adjacentFaces ()) {
        if (isInDomain (a)) {
          numInDomain++;
        }
      }
      return numInDomain >= 2;
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
      return hasAtLeast2NeighboursInDomain (face) || hasPoleVertex (face);
    };

    std::function <void (WingedFace&)> checkAdjacents = [&] (WingedFace& face) -> void {
      for (WingedFace& a : face.adjacentFaces ()) {
        if (isInDomain (a) == false && extendTo (a)) {
          extendedDomain.insert (&a);
          checkAdjacents        (a);
        }
      }
    };

    for (WingedFace* f : domain) {
      checkAdjacents (*f);
    }
    domain.insert (extendedDomain.begin (), extendedDomain.end ());
  }

  void run ( WingedMesh& mesh, const std::unordered_set <WingedEdge*>& selection
                             ,       std::unordered_set <WingedFace*>& affected ) 
  {
    SubdivideData data         (mesh, selection, affected);
    this->subdivideEdges       (data);
    this->triangulateFaces     (data);
    this->relaxByEdgeFlip      (data, 4);
    this->relaxByEdgeFlip      (data, 3);
    this->smoothAffectedFaces  (data);
  }

  void subdivideEdges (const SubdivideData& data) {
    for (WingedEdge* e : data.selection) {
      this->actions.add <PAInsertEdgeVertex> ()
                   .run (data.mesh, *e, SubdivisionButterfly::subdivideEdge (data.mesh, *e));
    }
  }

  void triangulateFaces (const SubdivideData& data) {
    for (WingedEdge* e : data.selection) {
      this->triangulateFace (data, e->leftFaceRef  ());
      this->triangulateFace (data, e->rightFaceRef ());
    }
  }

  void triangulateFace (const SubdivideData& data, WingedFace& face) {
    switch (face.numEdges ()) {
      case 3: break;
      case 4: this->actions.add <PATriangulateQuad> ()
                           .run (data.mesh, face, &data.affectedFaces);
              break;
      case 5: this->actions.add <PATriangulate5Gon> ()
                           .run (data.mesh, face, &data.affectedFaces);
              break;
      case 6: this->actions.add <PATriangulate6Gon> ()
                           .run (data.mesh, face, &data.affectedFaces);
              break;
      default: 
              assert (false);
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

  void relaxByEdgeFlip (const SubdivideData& data, int threshold) {
    FaceSet affectedFaces;
    for (WingedFace* f : data.affectedFaces) {
      for (WingedEdge* e : f->adjacentEdges ().collect ()) {
        if (this->relaxiationIndex (*e) >= threshold) {
          affectedFaces.insert (e->leftFace  ());
          affectedFaces.insert (e->rightFace ());
          actions.add <PAFlipEdge> ().run (*e);
        }
      }
    }
    data.affectedFaces.insert (affectedFaces.begin (), affectedFaces.end ());
  }

  int relaxiationIndex (const WingedEdge& edge) const {
    assert (edge.leftFaceRef  ().numEdges () == 3);
    assert (edge.rightFaceRef ().numEdges () == 3);

    return int (edge.vertex1Ref ().valence ())
         + int (edge.vertex2Ref ().valence ())
         - int (edge.vertexRef (edge.leftFaceRef  (), 2).valence ())
         - int (edge.vertexRef (edge.rightFaceRef (), 2).valence ());
  }
};

DELEGATE_BIG3 (ActionSubdivide)
DELEGATE1_STATIC (void, ActionSubdivide, extendDomain, std::vector<WingedFace*>&)
DELEGATE3 (void, ActionSubdivide, run, WingedMesh&, const std::unordered_set<WingedEdge*>&, std::unordered_set<WingedFace*>&)
DELEGATE1 (void, ActionSubdivide, runUndo, WingedMesh&)
DELEGATE1 (void, ActionSubdivide, runRedo, WingedMesh&)
