#include <unordered_set>
#include "partial-action/delete-t-edges.hpp"
#include "macro.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "action/unit.hpp"
#include "partial-action/delete-edge-face.hpp"

struct PADeleteTEdges :: Impl {
  ActionUnit actions;

  void run (WingedMesh& mesh, std::unordered_set <WingedFace*>& faces) {
    this->actions.reset ();

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

  void undo () { this->actions.undo (); }
  void redo () { this->actions.redo (); }
};

DELEGATE_CONSTRUCTOR (PADeleteTEdges)
DELEGATE_DESTRUCTOR  (PADeleteTEdges)

DELEGATE2 (void,PADeleteTEdges,run,WingedMesh&,std::unordered_set <WingedFace*>&)
DELEGATE  (void,PADeleteTEdges,undo)
DELEGATE  (void,PADeleteTEdges,redo)
