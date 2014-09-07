#include "tool/move.hpp"
#include "tool/movement.hpp"
#include "tool/new-sphere-mesh.hpp"
#include "view/sphere-mesh-menu.hpp"

ViewSphereMeshMenu :: ViewSphereMeshMenu (ViewMainWindow& mW, const glm::ivec2& p)
  : ViewToolMenu (mW,p) 
{
  this->addAction <ToolNewSphereMesh> ();
  this->addAction <ToolMove> (Movement::CameraPlane);
}
