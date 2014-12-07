#ifndef DILAY_VIEW_MENU_NO_SELECTION
#define DILAY_VIEW_MENU_NO_SELECTION

#include "view/tool/menu.hpp"

class ViewMenuNoSelection : public ViewToolMenu {
  public:
    ViewMenuNoSelection (ViewMainWindow&, const glm::ivec2&);
};

#endif
