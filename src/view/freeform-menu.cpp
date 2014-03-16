#include "view/freeform-menu.hpp"
#include "tool/new-freeform-mesh.hpp"

ViewFreeformMenu :: ViewFreeformMenu (ViewMainWindow* mW, QContextMenuEvent* mE)
  : ViewToolMenu (mW,mE) 
{
  this->addAction <ToolNewFreeformMesh> ();
}
