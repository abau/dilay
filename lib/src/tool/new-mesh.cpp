#include <glm/glm.hpp>
#include "history.hpp"
#include "mesh.hpp"
#include "mesh-util.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "winged/util.hpp"

struct ToolNewMesh::Impl {
  ToolNewMesh* self;

  Impl (ToolNewMesh* s) : self (s) {}

  ToolResponse runInitialize () const {
    this->self->snapshotScene ();

    WingedMesh& mesh = this->self->state ().scene ().newWingedMesh (MeshUtil::icosphere (3));

    WingedUtil::defaultScale (mesh);

    return ToolResponse::Terminate;
  }
};

DELEGATE_TOOL                 (ToolNewMesh)
DELEGATE_TOOL_RUN_INITIALIZE  (ToolNewMesh)
