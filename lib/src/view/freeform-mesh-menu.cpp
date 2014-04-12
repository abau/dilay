#include "view/freeform-mesh-menu.hpp"
#include "tool/new-freeform-mesh.hpp"

ViewFreeformMeshMenu :: ViewFreeformMeshMenu (ViewMainWindow* mW, QContextMenuEvent* mE)
  : ViewToolMenu (mW,mE) 
{
  this->addAction <ToolNewFreeformMesh> ();
}
