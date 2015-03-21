#include <glm/glm.hpp>
#include "history.hpp"
#include "mesh-definition.hpp"
#include "mesh.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tools.hpp"

struct ToolNewMesh::Impl {
  ToolNewMesh* self;

  Impl (ToolNewMesh* s) : self (s) {}

  bool runAllowUndoRedo () const { return false; }

  ToolResponse runInitialize () const {
    this->self->state ().scene ().newWingedMesh (MeshDefinition::icosphere (3));

    return ToolResponse::Terminate;
  }
};

DELEGATE_TOOL                 (ToolNewMesh)
DELEGATE_TOOL_RUN_INITIALIZE  (ToolNewMesh)
