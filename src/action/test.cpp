#include <glm/glm.hpp>
#include "action/test.hpp"
#include "macro.hpp"
#include "action/unit.hpp"
#include "winged-mesh.hpp"
#include "winged-face.hpp"
#include "winged-edge.hpp"
#include "partial-action/insert-edge-vertex.hpp"
#include "partial-action/triangulate-quad.hpp"
#include "subdivision-butterfly.hpp"

struct ActionTest :: Impl {
  ActionUnit actions;

  void run (WingedMesh& mesh, WingedEdge& edge) {
    this->actions.reset ();

    this->actions.add <PAInsertEdgeVertex> ()->run (mesh, edge
        , SubdivisionButterfly::subdivideEdge (mesh,1,edge));
    this->actions.add <PATriangulateQuad> ()->run (mesh, edge.leftFaceRef ());
    this->actions.add <PATriangulateQuad> ()->run (mesh, edge.rightFaceRef ());
  }

  void undo () { this->actions.undo (); }
  void redo () { this->actions.redo (); }
};

DELEGATE_CONSTRUCTOR (ActionTest)
DELEGATE_DESTRUCTOR  (ActionTest)

DELEGATE2 (void, ActionTest, run, WingedMesh&, WingedEdge&)
DELEGATE  (void, ActionTest, undo)
DELEGATE  (void, ActionTest, redo)
