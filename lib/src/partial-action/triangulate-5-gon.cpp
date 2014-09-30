#include "action/unit/on.hpp"
#include "partial-action/insert-edge-face.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "partial-action/triangulate-5-gon.hpp"
#include "partial-action/triangulate-quad.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"

struct PATriangulate5Gon :: Impl {
  ActionUnitOn <WingedMesh> actions;

  void run (WingedMesh& mesh, WingedFace& face, std::unordered_set <WingedFace*>* affectedFaces) {
    assert (face.numEdges () == 5);

    WingedEdge& newEdge = this->actions.add <PAInsertEdgeFace> ().run (mesh, face);

    this->actions.add <PAModifyWFace> ().writeIndices (mesh, newEdge.leftFaceRef  ());

    if (affectedFaces) {
      affectedFaces->insert (newEdge.leftFace  ());
    }
    this->actions.add <PATriangulateQuad> ().run (mesh, newEdge.rightFaceRef (), affectedFaces);
  }

  void runUndo (WingedMesh& mesh) { this->actions.undo (mesh); }
  void runRedo (WingedMesh& mesh) { this->actions.redo (mesh); }
};

DELEGATE_BIG3 (PATriangulate5Gon)

DELEGATE3 (void, PATriangulate5Gon, run, WingedMesh&, WingedFace&, std::unordered_set <WingedFace*>*)
DELEGATE1 (void, PATriangulate5Gon, runUndo, WingedMesh&)
DELEGATE1 (void, PATriangulate5Gon, runRedo, WingedMesh&)
