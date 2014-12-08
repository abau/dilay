#include "mesh-type.hpp"
#include "tool/util/movement.hpp"
#include "tools.hpp"
#include "view/menu/freeform-mesh.hpp"

ViewMenuFreeformMesh :: ViewMenuFreeformMesh (ViewMainWindow& mW, const glm::ivec2& p)
  : ViewToolMenu (mW,p) 
{
  this->addAction <ToolMove>            (MovementConstraint::CameraPlane);
  this->addAction <ToolCarve>           ();
  this->addAction <ToolSubdivideMesh>   ();
  this->addAction <ToolDeleteMesh>      (MeshType::Freeform);
  this->addSeparator                    ();
  this->addAction <ToolNewFreeformMesh> ();
}
