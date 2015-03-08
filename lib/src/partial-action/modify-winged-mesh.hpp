#ifndef DILAY_PARTIAL_ACTION_MODIFY_WINGED_MESH
#define DILAY_PARTIAL_ACTION_MODIFY_WINGED_MESH

#include <glm/fwd.hpp>
#include "action/on-winged-mesh.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedFace;
class WingedEdge;
class WingedVertex;
class PrimTriangle;

class PAModifyWMesh : public ActionOnWMesh {
  public: 
    DECLARE_BIG3 (PAModifyWMesh)

    void          deleteEdge      (WingedMesh&, WingedEdge&);
    void          deleteFace      (WingedMesh&, WingedFace&);
    void          deleteVertex    (WingedMesh&, WingedVertex&);
    WingedEdge&   addEdge         (WingedMesh&);
    WingedFace&   addFace         (WingedMesh&, const PrimTriangle&);
    WingedVertex& addVertex       (WingedMesh&, const glm::vec3&);
    void          setIndex        (WingedMesh&, unsigned int, unsigned int);
    void          setupOctreeRoot (WingedMesh&, const glm::vec3&, float);

  private:
    void runUndo (WingedMesh&) const;
    void runRedo (WingedMesh&) const;

    IMPLEMENTATION
};
#endif
