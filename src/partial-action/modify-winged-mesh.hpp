#ifndef DILAY_PARTIAL_ACTION_MODIFY_WINGED_MESH
#define DILAY_PARTIAL_ACTION_MODIFY_WINGED_MESH

#include "action/on-winged-mesh.hpp"
#include "fwd-glm.hpp"
#include "macro.hpp"

class WingedMesh;
class WingedFace;
class WingedEdge;
class WingedVertex;
class Triangle;

class PAModifyWMesh : public ActionOnWMesh {
  public: 
    DECLARE_ACTION_BIG5 (PAModifyWMesh)

    void          deleteEdge  (WingedMesh&, const WingedEdge&);
    void          deleteFace  (WingedMesh&, const WingedFace&);
    void          deleteFace  (WingedMesh&, const WingedFace&, const Triangle&);
    void          popVertex   (WingedMesh&);
    WingedEdge&   addEdge     (WingedMesh&, const WingedEdge&);
    WingedFace&   addFace     (WingedMesh&, const WingedFace&);
    WingedFace&   addFace     (WingedMesh&, const Triangle&);
    WingedFace&   addFace     (WingedMesh&, const WingedFace&, const Triangle&);
    WingedVertex& addVertex   (WingedMesh&, const glm::vec3&);
    WingedFace&   realignFace (WingedMesh&, const WingedFace&, const Triangle&);

    void undo (WingedMesh&);
    void redo (WingedMesh&);
  private:
    class Impl;
    Impl* impl;
};
#endif
