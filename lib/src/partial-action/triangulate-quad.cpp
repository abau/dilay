#include "action/unit/on.hpp"
#include "partial-action/insert-edge-face.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "partial-action/triangulate-quad.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"

struct PATriangulateQuad :: Impl {
  ActionUnitOn <WingedMesh> actions;

  WingedEdge& run ( WingedMesh& mesh, WingedFace& face
                  , std::unordered_set <WingedFace*>* affectedFaces ) 
  {
    assert (face.numEdges () == 4);

    WingedEdge& newEdge = this->actions.add <PAInsertEdgeFace> ().run (mesh, face);

    this->actions.add <PAModifyWFace> ().writeIndices (mesh, newEdge.leftFaceRef  ());
    this->actions.add <PAModifyWFace> ().writeIndices (mesh, newEdge.rightFaceRef ());

    if (affectedFaces) {
      affectedFaces->insert (newEdge.leftFace  ());
      affectedFaces->insert (newEdge.rightFace ());
    }
    return newEdge;
  }

  void runUndo (WingedMesh& mesh) { this->actions.undo (mesh); }
  void runRedo (WingedMesh& mesh) { this->actions.redo (mesh); }
};

DELEGATE_BIG3 (PATriangulateQuad)

DELEGATE3 (WingedEdge&, PATriangulateQuad, run, WingedMesh&, WingedFace&, std::unordered_set <WingedFace*>*)
DELEGATE1 (void, PATriangulateQuad, runUndo, WingedMesh&)
DELEGATE1 (void, PATriangulateQuad, runRedo, WingedMesh&)
