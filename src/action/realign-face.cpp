#include "action/realign-face.hpp"
#include "action/unit/on-winged-mesh.hpp"
#include "winged-mesh.hpp"
#include "winged-face.hpp"
#include "winged-edge.hpp"
#include "partial-action/modify-winged-mesh.hpp"
#include "partial-action/modify-winged-edge.hpp"
#include "adjacent-iterator.hpp"
#include "triangle.hpp"

struct ActionRealignFace :: Impl {
  ActionUnitOnWMesh actions;

  WingedFace& run (WingedMesh& mesh, const WingedFace& face) {
    assert (face.octreeNode ());

    std::list <WingedEdge*> adjacents = face.adjacentEdgeIterator ().collect ();
    Triangle                triangle  = face.triangle (mesh);

    for (WingedEdge* e : adjacents) {
      this->actions.add <PAModifyWEdge> ()->face (mesh,*e,face,nullptr);
    }

    WingedFace& newFace = this->actions.add <PAModifyWMesh> ()->realignFace (mesh, face, triangle);

    for (WingedEdge* e : adjacents) {
      if (e->leftFace () == nullptr)
        this->actions.add <PAModifyWEdge> ()->leftFace (mesh,*e,&newFace);
      else if (e->rightFace () == nullptr)
        this->actions.add <PAModifyWEdge> ()->rightFace (mesh,*e,&newFace);
      else
        assert (false);
    }
    return newFace;
  }

  void undo (WingedMesh& mesh) { this->actions.undo (mesh); }
  void redo (WingedMesh& mesh) { this->actions.redo (mesh); }
};

DELEGATE_ACTION_BIG5 (ActionRealignFace)
DELEGATE2 (WingedFace&, ActionRealignFace, run, WingedMesh&, const WingedFace&)
DELEGATE1 (void, ActionRealignFace, undo, WingedMesh&)
DELEGATE1 (void, ActionRealignFace, redo, WingedMesh&)
