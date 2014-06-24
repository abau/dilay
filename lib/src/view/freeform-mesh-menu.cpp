#include "view/freeform-mesh-menu.hpp"
#include "tool/delete-mesh.hpp"
#include "mesh-type.hpp"

ViewFreeformMeshMenu :: ViewFreeformMeshMenu (ViewMainWindow& mW, const glm::ivec2& p)
  : ViewToolMenu (mW,p) 
{
  this->addAction <ToolDeleteMesh> (MeshType::Freeform);
}
