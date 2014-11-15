#include <glm/glm.hpp>
#include "affected-faces.hpp"
#include "action/subdivide-mesh.hpp"
#include "action/unit/on.hpp"
#include "octree.hpp"
#include "partial-action/insert-edge-vertex.hpp"
#include "partial-action/triangulate-6-gon.hpp"
#include "subdivision-butterfly.hpp"
#include "winged/edge.hpp"
#include "winged/mesh.hpp"

struct ActionSubdivideMesh::Impl {
  ActionSubdivideMesh*      self;
  ActionUnitOn <WingedMesh> actions;

  Impl (ActionSubdivideMesh* s) : self (s) {}

  void runUndoBeforePostProcessing (WingedMesh& mesh) const { this->actions.undo (mesh); }
  void runRedoBeforePostProcessing (WingedMesh& mesh) const { this->actions.redo (mesh); }

  void run (WingedMesh& mesh) {
    AffectedFaces affected;
    mesh.octree ().forEachFace ([&affected] (WingedFace& f) {
      affected.insert (f);
    });
    affected.commit ();
    // subdivide edges
    for (WingedEdge* e : affected.edges ()) {
      this->actions.add <PAInsertEdgeVertex> ().run 
        (mesh, *e , SubdivisionButterfly::subdivideEdge (mesh, *e));
    }
    // triangulate faces
    for (WingedFace* f : affected.faces ()) {
      this->actions.add <PATriangulate6Gon>  ().run (mesh, *f, nullptr);
    }
    this->self->realignAllFaces (mesh);
    this->self->writeAllNormals (mesh);
    this->self->writeAllIndices (mesh);
    this->self->bufferData      (mesh);
  }
};

DELEGATE_BIG3_SELF (ActionSubdivideMesh)
DELEGATE1          (void, ActionSubdivideMesh, run, WingedMesh&)
DELEGATE1_CONST    (void, ActionSubdivideMesh, runUndoBeforePostProcessing, WingedMesh&)
DELEGATE1_CONST    (void, ActionSubdivideMesh, runRedoBeforePostProcessing, WingedMesh&)
