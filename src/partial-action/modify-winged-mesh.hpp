#ifndef DILAY_PARTIAL_ACTION_MODIFY_WINGED_MESH
#define DILAY_PARTIAL_ACTION_MODIFY_WINGED_MESH

#include <glm/fwd.hpp>
#include "action/on-winged-mesh.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedFace;
class WingedEdge;
class WingedVertex;
class Triangle;

class PAModifyWMesh : public ActionOnWMesh {
  public: 
    DECLARE_BIG3 (PAModifyWMesh)

    void          deleteEdge     (WingedMesh&, const WingedEdge&);
    void          deleteFace     (WingedMesh&, const WingedFace&, const Triangle&);
    void          popVertex      (WingedMesh&);
    WingedEdge&   addEdge        (WingedMesh&, const WingedEdge&);
    WingedFace&   addFace        (WingedMesh&, const WingedFace&);
    WingedFace&   addFace        (WingedMesh&, const Triangle&);
    WingedFace&   addFace        (WingedMesh&, const WingedFace&, const Triangle&);
    WingedVertex& addVertex      (WingedMesh&, const glm::vec3&);
    void          initOctreeRoot (WingedMesh&, const glm::vec3&, float);

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    class Impl;
    Impl* impl;
};
#endif
