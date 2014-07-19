#include "view/freeform-mesh-menu.hpp"
#include "tool/delete-mesh.hpp"
#include "tool/new-freeform-mesh.hpp"
#include "tool/movement.hpp"
#include "tool/move.hpp"
#include "mesh-type.hpp"

ViewFreeformMeshMenu :: ViewFreeformMeshMenu (ViewMainWindow& mW, const glm::ivec2& p)
  : ViewToolMenu (mW,p) 
{
  this->addAction <ToolDeleteMesh>      (MeshType::Freeform);
  this->addAction <ToolNewFreeformMesh> ();
  this->addAction <ToolMove>            (Movement::CameraPlane);
}
