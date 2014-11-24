#include <glm/glm.hpp>
#include "action/unit/on.hpp"
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "partial-action/insert-edge-vertex.hpp"
#include "partial-action/triangulate-quad.hpp"
#include "partial-action/subdivide-edge.hpp"
#include "subdivision-butterfly.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/vertex.hpp"

struct PASubdivideEdge::Impl {
  ActionUnitOn <WingedMesh> actions;

  void runUndo (WingedMesh& mesh) const { this->actions.undo (mesh); }
  void runRedo (WingedMesh& mesh) const { this->actions.redo (mesh); }

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

  void run (WingedMesh& mesh, WingedEdge& edge, AffectedFaces& affectedFaces) {
    this->actions.add <PAInsertEdgeVertex> ().run 
      (mesh, edge, SubdivisionButterfly::subdivideEdge (mesh, edge) );
    this->actions.add <PATriangulateQuad>  ().run (mesh, edge.leftFaceRef  (), &affectedFaces);
    this->actions.add <PATriangulateQuad>  ().run (mesh, edge.rightFaceRef (), &affectedFaces);
  }
};

DELEGATE_BIG3 (PASubdivideEdge)
DELEGATE1_STATIC (void, PASubdivideEdge, extendDomain, AffectedFaces&)
DELEGATE3        (void, PASubdivideEdge, run, WingedMesh&, WingedEdge&, AffectedFaces&)
DELEGATE1_CONST  (void, PASubdivideEdge, runUndo, WingedMesh&)
DELEGATE1_CONST  (void, PASubdivideEdge, runRedo, WingedMesh&)
