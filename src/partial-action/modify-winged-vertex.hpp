#ifndef DILAY_PARTIAL_ACTION_MODIFY_WINGED_VERTEX
#define DILAY_PARTIAL_ACTION_MODIFY_WINGED_VERTEX

#include "action.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedEdge;
class WingedVertex;

class PAModifyWVertex : public Action {
  public: 
    DECLARE_ACTION_BIG5 (PAModifyWVertex)

    void edge        (WingedMesh&, WingedVertex&, WingedEdge*);
    void writeIndex  (WingedMesh&, WingedVertex&, unsigned int);
    void writeNormal (WingedMesh&, WingedVertex&);
    void move        (WingedMesh&, WingedVertex&, const glm::vec3&);

    void undo ();
    void redo ();
  private:
    class Impl;
    Impl* impl;
};
#endif
