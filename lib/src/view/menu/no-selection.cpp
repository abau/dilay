#include "tools.hpp"
#include "view/menu/no-selection.hpp"

ViewMenuNoSelection :: ViewMenuNoSelection (State& s, const glm::ivec2& p) 
  : ViewToolMenu (s, p) 
{
  this->addAction <ToolNewWingedMesh> (QObject::tr ("New freeform mesh"));
}
