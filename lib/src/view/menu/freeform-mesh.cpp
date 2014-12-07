#include "mesh-type.hpp"
#include "tool/util/movement.hpp"
#include "tools.hpp"
#include "view/menu/freeform-mesh.hpp"

ViewMenuFreeformMesh :: ViewMenuFreeformMesh (ViewMainWindow& mW, const glm::ivec2& p)
  : ViewToolMenu (mW,p) 
{
  this->addAction <ToolDeleteMesh>      (MeshType::Freeform);
  this->addAction <ToolNewFreeformMesh> ();
  this->addAction <ToolMove>            (MovementPlane::Camera);
  this->addAction <ToolCarve>           ();
  this->addAction <ToolSubdivideMesh>   ();
}
