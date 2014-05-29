#ifndef DILAY_VIEW_FREEFORM_MESH_MENU
#define DILAY_VIEW_FREEFORM_MESH_MENU

#include "view/tool-menu.hpp"

class ViewFreeformMeshMenu : public ViewToolMenu {
  public:
    ViewFreeformMeshMenu (ViewMainWindow&, QContextMenuEvent&);
};

#endif
