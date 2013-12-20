#ifndef DILAY_PARTIAL_ACTION_MODIFY_MESH
#define DILAY_PARTIAL_ACTION_MODIFY_MESH

#include "action.hpp"
#include "fwd-glm.hpp"

class WingedMesh;
class WingedFace;
class WingedEdge;
class WingedVertex;
class Triangle;

class PAModifyMesh : public Action {
  public: PAModifyMesh            ();
          PAModifyMesh            (const PAModifyMesh&) = delete;
    const PAModifyMesh& operator= (const PAModifyMesh&) = delete;
         ~PAModifyMesh            ();

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

    void undo ();
    void redo ();
  private:
    class Impl;
    Impl* impl;
};
#endif
