#include "mesh-type.hpp"
#include "tool/util/movement.hpp"
#include "tools.hpp"
#include "view/menu/no-selection.hpp"

ViewMenuNoSelection :: ViewMenuNoSelection (ViewMainWindow& mW, const glm::ivec2& p)
  : ViewToolMenu (mW,p) 
{
  this->addAction <ToolNewFreeformMesh> ();
}
