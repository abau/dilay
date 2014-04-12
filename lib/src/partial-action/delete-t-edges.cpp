#include <unordered_set>
#include "partial-action/delete-t-edges.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "action/unit/on.hpp"
#include "partial-action/delete-edge-face.hpp"

struct PADeleteTEdges :: Impl {
  ActionUnitOn <WingedMesh> actions;

  void run (WingedMesh& mesh, std::unordered_set <WingedFace*>& faces) {
    assert (this->actions.isEmpty ());

    for (auto it = faces.begin (); it != faces.end (); ) {
      auto        faceIt = it;
      WingedFace* face   = *it;
      ++it;

      WingedEdge* tEdge = face->tEdge ();
      if (tEdge && tEdge->isRightFace (*face)) {
        WingedFace& leftFace = tEdge->leftFaceRef ();

        this->actions.add <PADeleteEdgeFace> ()->run (mesh,*tEdge);
        faces.erase (faceIt);
        assert (leftFace.tEdge () == nullptr);
      }
    }
  }

  void runUndo (WingedMesh& mesh) { this->actions.undo (mesh); }
  void runRedo (WingedMesh& mesh) { this->actions.redo (mesh); }
};

DELEGATE_BIG3 (PADeleteTEdges)

DELEGATE2 (void,PADeleteTEdges,run,WingedMesh&,std::unordered_set <WingedFace*>&)
DELEGATE1 (void,PADeleteTEdges,runUndo,WingedMesh&)
DELEGATE1 (void,PADeleteTEdges,runRedo,WingedMesh&)
