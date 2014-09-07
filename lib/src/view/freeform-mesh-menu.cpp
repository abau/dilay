#include "mesh-type.hpp"
#include "tool/carve.hpp"
#include "tool/delete-mesh.hpp"
#include "tool/move.hpp"
#include "tool/movement.hpp"
#include "tool/new-freeform-mesh.hpp"
#include "view/freeform-mesh-menu.hpp"

ViewFreeformMeshMenu :: ViewFreeformMeshMenu (ViewMainWindow& mW, const glm::ivec2& p)
  : ViewToolMenu (mW,p) 
{
  this->addAction <ToolDeleteMesh>      (MeshType::Freeform);
  this->addAction <ToolNewFreeformMesh> ();
  this->addAction <ToolMove>            (Movement::CameraPlane);
  this->addAction <ToolCarve>           ();
}
