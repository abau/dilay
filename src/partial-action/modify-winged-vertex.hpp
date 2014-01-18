#ifndef DILAY_PARTIAL_ACTION_MODIFY_WINGED_VERTEX
#define DILAY_PARTIAL_ACTION_MODIFY_WINGED_VERTEX

#include "action/on-winged-mesh.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedEdge;
class WingedVertex;

class PAModifyWVertex : public ActionOnWMesh {
  public: 
    DECLARE_ACTION_BIG6 (PAModifyWVertex)

    void edge        (WingedVertex&, WingedEdge*);
    void writeIndex  (WingedMesh&, WingedVertex&, unsigned int);
    void writeNormal (WingedMesh&, WingedVertex&);
    void move        (WingedMesh&, WingedVertex&, const glm::vec3&);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    class Impl;
    Impl* impl;
};
#endif
