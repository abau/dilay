#include <QObject>
#include "action/subdivide-mesh.hpp"
#include "action/unit.hpp"
#include "history.hpp"
#include "scene.hpp"
#include "selection.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "winged/mesh.hpp"

struct ToolSubdivideMesh::Impl {
  ToolSubdivideMesh* self;

  Impl (ToolSubdivideMesh* s) : self (s) {}

  ToolResponse runInitialize () {
    Scene&     scene = this->self->state ().scene ();
    ActionUnit unit;

    scene.selection ().forEachMajor ([&scene,&unit] (unsigned int index) {
      WingedMesh& mesh = scene.wingedMeshRef (index);
      unit.add <ActionSubdivideMesh> (scene, mesh).run (mesh);
    });

    if (unit.isEmpty () == false) {
      this->self->state ().history ().addUnit (std::move (unit));
    }
    return ToolResponse::Terminate;
  }
};

DELEGATE_TOOL                (ToolSubdivideMesh)
DELEGATE_TOOL_RUN_INITIALIZE (ToolSubdivideMesh)
