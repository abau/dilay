#ifndef DILAY_PARTIAL_ACTION_MODIFY_WINGED_VERTEX
#define DILAY_PARTIAL_ACTION_MODIFY_WINGED_VERTEX

#include <glm/fwd.hpp>
#include "action/on-winged-mesh.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedEdge;
class WingedVertex;

class PAModifyWVertex : public ActionOnWMesh {
  public: 
    DECLARE_BIG3 (PAModifyWVertex)

    void edge                    (WingedVertex&, WingedEdge*);
    void reset                   (WingedVertex&);
    void writeIndex              (WingedMesh&, WingedVertex&, unsigned int);
    void writeNormal             (WingedMesh&, WingedVertex&, const glm::vec3&);
    void writeInterpolatedNormal (WingedMesh&, WingedVertex&);
    void move                    (WingedMesh&, WingedVertex&, const glm::vec3&);
    void moved                   (WingedMesh&, WingedVertex&, const glm::vec3&);

  private:
    void runUndo (WingedMesh&) const;
    void runRedo (WingedMesh&) const;

    IMPLEMENTATION
};
#endif
