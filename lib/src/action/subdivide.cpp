#include <glm/glm.hpp>
#include "affected-faces.hpp"
#include "action/subdivide.hpp"
#include "action/unit/on.hpp"
#include "adjacent-iterator.hpp"
#include "partial-action/flip-edge.hpp"
#include "partial-action/insert-edge-vertex.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "partial-action/triangulate-quad.hpp"
#include "subdivision-butterfly.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/vertex.hpp"

struct ActionSubdivide::Impl {
  ActionUnitOn <WingedMesh> actions;

  void runUndo (WingedMesh& mesh) { this->actions.undo (mesh); }
  void runRedo (WingedMesh& mesh) { this->actions.redo (mesh); }

  static void extendDomain (AffectedFaces& domain) {
    Impl::addOneRing            (domain);
    Impl::addOneRing            (domain);
    Impl::extendToNeighbourhood (domain);
  }

  static void addOneRing (AffectedFaces& domain) {
    for (WingedFace* d : domain.faces ()) {
      for (WingedFace& f : d->adjacentFaces ()) {
        domain.insert (f);
      }
    }
    domain.commit ();
  }

  static void extendToNeighbourhood (AffectedFaces& domain) {
    auto hasAtLeast2NeighboursInDomain = [&domain] (WingedFace& face) -> bool {
      unsigned int numInDomain = 0;

      for (WingedFace& a : face.adjacentFaces ()) {
        if (domain.contains (a)) {
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
        if (domain.contains (a) == false && extendTo (a)) {
          domain.insert  (a);
          checkAdjacents (a);
        }
      }
    };

    for (WingedFace* f : domain.faces ()) {
      checkAdjacents (*f);
    }
    domain.commit ();
  }

  void subdivideEdge (WingedMesh& mesh, WingedEdge& edge, AffectedFaces& affectedFaces) {
    this->actions.add <PAInsertEdgeVertex> ().run 
      (mesh, edge/*, SubdivisionButterfly::subdivideEdge (mesh, edge) ); */, edge.middle (mesh));
    this->actions.add <PATriangulateQuad>  ().run (mesh, edge.leftFaceRef  (), &affectedFaces);
    this->actions.add <PATriangulateQuad>  ().run (mesh, edge.rightFaceRef (), &affectedFaces);
  }

  void relaxEdge (WingedMesh& mesh, WingedEdge& edge, AffectedFaces& affectedFaces) {
    if (this->relaxableEdge (edge)) {
      affectedFaces.insert (edge.leftFaceRef  ());
      affectedFaces.insert (edge.rightFaceRef ());
      this->actions.add <PAFlipEdge> ().run (mesh, edge);
    }
  }

  bool relaxableEdge (const WingedEdge& edge) const {
    const int v1  = int (edge.vertex1Ref ().valence ());
    const int v2  = int (edge.vertex2Ref ().valence ());
    const int v3  = int (edge.vertexRef (edge.leftFaceRef  (), 2).valence ());
    const int v4  = int (edge.vertexRef (edge.rightFaceRef (), 2).valence ());

    const int pre  = glm::abs (v1-6)   + glm::abs (v2-6)   + glm::abs (v3-6)   + glm::abs (v4-6);
    const int post = glm::abs (v1-6-1) + glm::abs (v2-6-1) + glm::abs (v3-6+1) + glm::abs (v4-6+1);

    return post < pre;
  }
};

DELEGATE_BIG3 (ActionSubdivide)
DELEGATE1_STATIC (void, ActionSubdivide, extendDomain, AffectedFaces&)
DELEGATE1_STATIC (void, ActionSubdivide, addOneRing, AffectedFaces&)
DELEGATE3 (void, ActionSubdivide, subdivideEdge, WingedMesh&, WingedEdge&, AffectedFaces&)
DELEGATE3 (void, ActionSubdivide, relaxEdge, WingedMesh&, WingedEdge&, AffectedFaces&)
DELEGATE1 (void, ActionSubdivide, runUndo, WingedMesh&)
DELEGATE1 (void, ActionSubdivide, runRedo, WingedMesh&)
