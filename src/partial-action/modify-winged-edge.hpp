#ifndef DILAY_PARTIAL_ACTION_MODIFY_WINGED_EDGE
#define DILAY_PARTIAL_ACTION_MODIFY_WINGED_EDGE

#include "action/on-winged-mesh.hpp"
#include "fwd-glm.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedEdge;
enum class FaceGradient : char;
class WingedFace;
class WingedVertex;

class PAModifyWEdge : public ActionOnWMesh {
  public: 
    DECLARE_ACTION_BIG5 (PAModifyWEdge)

    void vertex1              (WingedMesh&, WingedEdge&, WingedVertex*);
    void vertex2              (WingedMesh&, WingedEdge&, WingedVertex*);
    void leftFace             (WingedMesh&, WingedEdge&, WingedFace*);
    void rightFace            (WingedMesh&, WingedEdge&, WingedFace*);
    void leftPredecessor      (WingedMesh&, WingedEdge&, WingedEdge*);
    void leftSuccessor        (WingedMesh&, WingedEdge&, WingedEdge*);
    void rightPredecessor     (WingedMesh&, WingedEdge&, WingedEdge*);
    void rightSuccessor       (WingedMesh&, WingedEdge&, WingedEdge*);
    void previousSibling      (WingedMesh&, WingedEdge&, WingedEdge*);
    void nextSibling          (WingedMesh&, WingedEdge&, WingedEdge*);
    void firstVertex          (WingedMesh&, WingedEdge&, const WingedFace&, WingedVertex*);
    void secondVertex         (WingedMesh&, WingedEdge&, const WingedFace&, WingedVertex*);
    void face                 (WingedMesh&, WingedEdge&, const WingedFace&, WingedFace*);
    void predecessor          (WingedMesh&, WingedEdge&, const WingedFace&, WingedEdge*);
    void successor            (WingedMesh&, WingedEdge&, const WingedFace&, WingedEdge*);
    void setGeometry          ( WingedMesh&, WingedEdge&, WingedVertex*, WingedVertex*
                              , WingedFace*, WingedFace*, WingedEdge*, WingedEdge*
                              , WingedEdge*, WingedEdge*
                              , WingedEdge* = nullptr, WingedEdge* = nullptr);
    void isTEdge              (WingedMesh&, WingedEdge&, bool);
    void faceGradient         (WingedMesh&, WingedEdge&, FaceGradient);
    void increaseFaceGradient (WingedMesh&, WingedEdge&, const WingedFace&);
    void vertexGradient       (WingedMesh&, WingedEdge&, int);

    void undo (WingedMesh&);
    void redo (WingedMesh&);
  private:
    class Impl;
    Impl* impl;
};
#endif
