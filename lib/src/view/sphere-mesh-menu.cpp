#include "view/sphere-mesh-menu.hpp"
#include "tool/new-sphere-mesh.hpp"

ViewSphereMeshMenu :: ViewSphereMeshMenu (ViewMainWindow& mW, const glm::ivec2& p)
  : ViewToolMenu (mW,p) 
{
  this->addAction <ToolNewSphereMesh> ();
}
