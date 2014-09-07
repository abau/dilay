#ifndef DILAY_PARTIAL_ACTION_MODIFY_WINGED_VERTEX
#define DILAY_PARTIAL_ACTION_MODIFY_WINGED_VERTEX

#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedEdge;
class WingedVertex;

class PAModifyWVertex : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (PAModifyWVertex)

    void edge        (WingedVertex&, WingedEdge*);
    void writeIndex  (WingedMesh&, WingedVertex&, unsigned int);
    void writeNormal (WingedMesh&, WingedVertex&, const glm::vec3&);
    void move        (WingedMesh&, WingedVertex&, const glm::vec3&);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    IMPLEMENTATION
};
#endif
