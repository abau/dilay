#ifndef DILAY_VIEW_FREEFORM_MENU
#define DILAY_VIEW_FREEFORM_MENU

#include "view/tool-menu.hpp"

class ViewFreeformMenu : public ViewToolMenu {
  public:
    ViewFreeformMenu (ViewMainWindow*, QContextMenuEvent*);
};

#endif
