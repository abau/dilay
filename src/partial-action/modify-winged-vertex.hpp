#ifndef DILAY_PARTIAL_ACTION_MODIFY_WINGED_VERTEX
#define DILAY_PARTIAL_ACTION_MODIFY_WINGED_VERTEX

#include "action.hpp"

class WingedMesh;
class WingedEdge;
class WingedVertex;

class PAModifyWVertex : public Action {
  public: PAModifyWVertex            ();
          PAModifyWVertex            (const PAModifyWVertex&) = delete;
    const PAModifyWVertex& operator= (const PAModifyWVertex&) = delete;
         ~PAModifyWVertex            ();

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
