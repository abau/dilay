#ifndef DILAY_PARTIAL_ACTION_MODIFY_WINGED_MESH
#define DILAY_PARTIAL_ACTION_MODIFY_WINGED_MESH

#include <glm/fwd.hpp>
#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedFace;
class WingedEdge;
class WingedVertex;
class PrimTriangle;

class PAModifyWMesh : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (PAModifyWMesh)

    void          deleteEdge      (WingedMesh&, const WingedEdge&);
    void          deleteFace      (WingedMesh&, const WingedFace&, const PrimTriangle&);
    void          deleteVertex    (WingedMesh&, const WingedVertex&);
    WingedEdge&   addEdge         (WingedMesh&, WingedEdge&&);
    WingedFace&   addFace         (WingedMesh&, WingedFace&&);
    WingedFace&   addFace         (WingedMesh&, const PrimTriangle&);
    WingedFace&   addFace         (WingedMesh&, WingedFace&&, const PrimTriangle&);
    WingedVertex& addVertex       (WingedMesh&, const glm::vec3&);
    void          setupOctreeRoot (WingedMesh&, const glm::vec3&, float);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    IMPLEMENTATION
};
#endif
