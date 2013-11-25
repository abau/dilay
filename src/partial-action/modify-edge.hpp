#ifndef DILAY_PARTIAL_ACTION_MODIFY_EDGE
#define DILAY_PARTIAL_ACTION_MODIFY_EDGE

#include "action.hpp"
#include "fwd-glm.hpp"

class WingedMesh;
class WingedEdge;
enum class WEGradient : char;
class WingedFace;
class WingedVertex;

class PAModifyEdge : public Action {
  public: PAModifyEdge            ();
          PAModifyEdge            (const PAModifyEdge&) = delete;
    const PAModifyEdge& operator= (const PAModifyEdge&) = delete;
         ~PAModifyEdge            ();

    void vertex1          (WingedMesh&, WingedEdge&, WingedVertex*);
    void vertex2          (WingedMesh&, WingedEdge&, WingedVertex*);
    void leftFace         (WingedMesh&, WingedEdge&, WingedFace*);
    void rightFace        (WingedMesh&, WingedEdge&, WingedFace*);
    void leftPredecessor  (WingedMesh&, WingedEdge&, WingedEdge*);
    void leftSuccessor    (WingedMesh&, WingedEdge&, WingedEdge*);
    void rightPredecessor (WingedMesh&, WingedEdge&, WingedEdge*);
    void rightSuccessor   (WingedMesh&, WingedEdge&, WingedEdge*);
    void previousSibling  (WingedMesh&, WingedEdge&, WingedEdge*);
    void nextSibling      (WingedMesh&, WingedEdge&, WingedEdge*);
    void firstVertex      (WingedMesh&, WingedEdge&, const WingedFace&, WingedVertex*);
    void secondVertex     (WingedMesh&, WingedEdge&, const WingedFace&, WingedVertex*);
    void face             (WingedMesh&, WingedEdge&, const WingedFace&, WingedFace*);
    void predecessor      (WingedMesh&, WingedEdge&, const WingedFace&, WingedEdge*);
    void successor        (WingedMesh&, WingedEdge&, const WingedFace&, WingedEdge*);
    void setGeometry      ( WingedMesh&, WingedEdge&, WingedVertex*, WingedVertex*
                          , WingedFace*, WingedFace*, WingedEdge*, WingedEdge*
                          , WingedEdge*, WingedEdge*
                          , WingedEdge* = nullptr, WingedEdge* = nullptr);
    void isTEdge          (WingedMesh&, WingedEdge&, bool);
    void gradient         (WingedMesh&, WingedEdge&, WEGradient);
    void gradient         (WingedMesh&, WingedEdge&, const WingedFace&);

    void undo ();
    void redo ();
  private:
    class Impl;
    Impl* impl;
};
#endif
