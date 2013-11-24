#ifndef DILAY_PARTIAL_ACTION_MODIFY_VERTEX
#define DILAY_PARTIAL_ACTION_MODIFY_VERTEX

#include "action.hpp"

class WingedMesh;
class WingedEdge;
class WingedVertex;

class PAModifyVertex : public Action {
  public: PAModifyVertex            ();
          PAModifyVertex            (const PAModifyVertex&) = delete;
    const PAModifyVertex& operator= (const PAModifyVertex&) = delete;
         ~PAModifyVertex            ();

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
