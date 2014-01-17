#ifndef DILAY_PARTIAL_ACTION_MODIFY_WINGED_EDGE
#define DILAY_PARTIAL_ACTION_MODIFY_WINGED_EDGE

#include "action/on-winged-mesh.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedEdge;
enum class FaceGradient : char;
class WingedFace;
class WingedVertex;

class PAModifyWEdge : public ActionOnWMesh {
  public: 
    DECLARE_ACTION_BIG6 (PAModifyWEdge)

    void vertex1              (WingedEdge&, WingedVertex*);
    void vertex2              (WingedEdge&, WingedVertex*);
    void leftFace             (WingedEdge&, WingedFace*);
    void rightFace            (WingedEdge&, WingedFace*);
    void leftPredecessor      (WingedEdge&, WingedEdge*);
    void leftSuccessor        (WingedEdge&, WingedEdge*);
    void rightPredecessor     (WingedEdge&, WingedEdge*);
    void rightSuccessor       (WingedEdge&, WingedEdge*);
    void previousSibling      (WingedEdge&, WingedEdge*);
    void nextSibling          (WingedEdge&, WingedEdge*);
    void firstVertex          (WingedEdge&, const WingedFace&, WingedVertex*);
    void secondVertex         (WingedEdge&, const WingedFace&, WingedVertex*);
    void face                 (WingedEdge&, const WingedFace&, WingedFace*);
    void predecessor          (WingedEdge&, const WingedFace&, WingedEdge*);
    void successor            (WingedEdge&, const WingedFace&, WingedEdge*);
    void setGeometry          ( WingedEdge&, WingedVertex*, WingedVertex*
                              , WingedFace*, WingedFace*, WingedEdge*, WingedEdge*
                              , WingedEdge*, WingedEdge*
                              , WingedEdge* = nullptr, WingedEdge* = nullptr);
    void isTEdge              (WingedEdge&, bool);
    void faceGradient         (WingedEdge&, FaceGradient);
    void increaseFaceGradient (WingedEdge&, const WingedFace&);
    void vertexGradient       (WingedEdge&, int);

    void undo (WingedMesh&);
    void redo (WingedMesh&);
  private:
    class Impl;
    Impl* impl;
};
#endif
