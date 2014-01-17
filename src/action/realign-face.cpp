#include "action/realign-face.hpp"
#include "winged-mesh.hpp"
#include "winged-face.hpp"
#include "winged-edge.hpp"
#include "adjacent-iterator.hpp"
#include "action/ids.hpp"
#include "triangle.hpp"

WingedFace& ActionRealignFace :: runStatic (WingedMesh& mesh, const WingedFace& face) {
  assert (face.octreeNode ());

  Triangle faceTriangle (face.triangle (mesh));
  std::list <WingedEdge*> adjacents = face.adjacentEdgeIterator ().collect ();

  for (WingedEdge* e : adjacents) {
    e->face (face,nullptr);
  }

  WingedFace& newFace = mesh.realignFace (face, faceTriangle);

  for (WingedEdge* e : adjacents) {
    if (e->leftFace () == nullptr)
      e->leftFace (&newFace);
    else if (e->rightFace () == nullptr)
      e->rightFace (&newFace);
    else
      assert (false);
  }
  return newFace;
}

struct ActionRealignFace :: Impl {
  ActionIds operand;

  WingedFace& run (WingedMesh& mesh, const WingedFace& face) {
    this->operand.setFace (0,&face);
    return ActionRealignFace::runStatic (mesh, face);
  }

  void undo (WingedMesh& mesh) { 
    WingedFace* face = this->operand.getFace (mesh,0);

    if (face) {
      this->run (mesh, *face); 
    }
  }

  void redo (WingedMesh& mesh) { 
    WingedFace& face = this->operand.getFaceRef (mesh,0);
    this->run (mesh, face); 
  }
};

DELEGATE_ACTION_BIG6 (ActionRealignFace)
DELEGATE2 (WingedFace&, ActionRealignFace, run, WingedMesh&, const WingedFace&)
DELEGATE1 (void, ActionRealignFace, undo, WingedMesh&)
DELEGATE1 (void, ActionRealignFace, redo, WingedMesh&)
