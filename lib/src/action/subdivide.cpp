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
  typedef std::unordered_set <WingedFace*>   FaceSet;
  typedef std::unordered_set <WingedEdge*>   EdgeSet;
  typedef std::unordered_set <WingedVertex*> VertexSet;

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
    EdgeSet tEdges;

    SubdivideData data        (mesh, selection, affected);
    this->subdivideEdges      (data);
    this->triangulateFaces    (data, tEdges);
    this->relaxByEdgeFlip     (data, tEdges);
    this->smoothAffectedFaces (data);
  }

  void subdivideEdges (const SubdivideData& data) {
    for (WingedEdge* e : data.selection) {
      this->actions.add <PAInsertEdgeVertex> ()
                   .run (data.mesh, *e, e->middle (data.mesh));
      //SubdivisionButterfly::subdivideEdge (data.mesh, *e));
    }
  }

  void triangulateFaces (const SubdivideData& data, EdgeSet& tEdges) {
    for (WingedEdge* e : data.selection) {
      this->triangulateFace (data, tEdges, e->leftFaceRef  ());
      this->triangulateFace (data, tEdges, e->rightFaceRef ());
    }
  }

  void triangulateFace (const SubdivideData& data, EdgeSet& tEdges, WingedFace& face) {
    switch (face.numEdges ()) {
      case 3: break;
      case 4: tEdges.insert (&this->actions.add <PATriangulateQuad> ()
                                           .run (data.mesh, face, &data.affectedFaces));
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
    std::unordered_set <WingedVertex*> vertices;

    for (WingedFace* face : data.affectedFaces) {
      assert (face->isTriangle ());
      for (WingedVertex& v : face->adjacentVertices ()) {
        vertices.insert (&v);
      }
    }

    std::unordered_map <WingedVertex*,glm::vec3> deltas;
    // compute deltas
    for (WingedVertex* v : vertices) {
      const glm::vec3     p       (v->vector (data.mesh));
            glm::vec3     delta   (0.0f);
            unsigned int  valence (0);

      for (WingedEdge& e : v->adjacentEdges ()) {
        delta += e.otherVertexRef (*v).vector (data.mesh) - p;
        valence++;
      }
      deltas.emplace (v, delta / float (valence));
    }

    std::unordered_map <WingedVertex*,glm::vec3> positions;

    for (auto deltaIt : deltas) {
      WingedVertex& v (*deltaIt.first);
      glm::vec3     normal  (0.0f);
      unsigned int  valence (0);

      for (WingedFace& f : v.adjacentFaces ()) {
        normal += f.triangle (data.mesh).normal ();
        valence++;
      }
      normal *= 1.0f / float (valence);

      const glm::vec3 p     (v.vector (data.mesh));
      const glm::vec3 delta (deltaIt.second);
            float     dot   (glm::dot (normal, delta));

      positions.emplace (&v, (p + delta) - (normal * dot));
    }
    for (auto it : positions) {
      this->actions.add <PAModifyWVertex> ().move (data.mesh, *it.first, it.second);
    }
  }

  void relaxByEdgeFlip (const SubdivideData& data, EdgeSet& tEdges) {
    VertexSet vertices;
    for (WingedEdge* edge : data.selection) {
      assert (edge->leftFaceRef  ().numEdges () == 3);
      assert (edge->rightFaceRef ().numEdges () == 3);

      vertices.insert (edge->vertex1 ());
    }
    this->relaxByEdgeFlip (data, tEdges, vertices, 4);
    this->relaxByEdgeFlip (data, tEdges, vertices, 3);
  }

  void relaxByEdgeFlip ( const SubdivideData& data, const EdgeSet& tEdges
                       , const VertexSet& vertices, int threshold) 
  {
    auto relaxEdge = [this,&tEdges,&vertices,threshold] (WingedEdge& e) -> int {
      const int     rI = this->relaxiationIndex (e);
      WingedVertex* v1 = e.vertex (e.leftFaceRef  (), 2);
      WingedVertex* v2 = e.vertex (e.rightFaceRef (), 2);

      return (rI >= threshold) 
          && (vertices.count (v1) + vertices.count (v2) == 1) 
          && (vertices.count (e.vertex1 ()) + vertices.count (e.vertex2 ()) == 1) 
          && (tEdges.count (&e) == 0)
          ?  rI : 0;
    };

    FaceSet affectedFaces;
    for (WingedFace* f : data.affectedFaces) {
      int         maxRelaxiationIndex = 0;
      WingedEdge* edgeToRelax         = nullptr;

      for (WingedEdge& e : f->adjacentEdges ()) {
        const int rI = relaxEdge (e);
        if (rI > maxRelaxiationIndex) {
          maxRelaxiationIndex = rI;
          edgeToRelax         = &e;
        }
      }
      if (maxRelaxiationIndex >= threshold) {
        assert (edgeToRelax);
        affectedFaces.insert (edgeToRelax->leftFace  ());
        affectedFaces.insert (edgeToRelax->rightFace ());
        actions.add <PAFlipEdge> ().run (data.mesh, *edgeToRelax);
      }
    }
    data.affectedFaces.insert (affectedFaces.begin (), affectedFaces.end ());
  }

  int relaxiationIndex (const WingedEdge& edge) const {
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
