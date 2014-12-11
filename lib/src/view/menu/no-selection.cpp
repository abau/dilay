#include "tools.hpp"
#include "view/menu/no-selection.hpp"

ViewMenuNoSelection :: ViewMenuNoSelection (const glm::ivec2& p) : ViewToolMenu (p) {
  this->addAction <ToolNewWingedMesh> (QObject::tr ("New PolyMesh"));
}
