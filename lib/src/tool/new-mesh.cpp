#include <glm/glm.hpp>
#include "action/new-winged-mesh.hpp"
#include "history.hpp"
#include "mesh-definition.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "util.hpp"

struct ToolNewMesh::Impl {
  ToolNewMesh* self;

  Impl (ToolNewMesh* s) : self (s) {}

  bool runAllowUndoRedo () const { return false; }

  ToolResponse runInitialize () const {
    MeshDefinition meshDefinition (MeshDefinition::Icosphere (3));
    meshDefinition.scale          (glm::vec3 (Util::defaultScale ()));

    Action::newWingedMesh (this->self->state ().scene (), meshDefinition);

    return ToolResponse::Terminate;
  }
};

DELEGATE_TOOL                 (ToolNewMesh)
DELEGATE_TOOL_RUN_INITIALIZE  (ToolNewMesh)
