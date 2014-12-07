#ifndef DILAY_VIEW_MENU_FREEFORM_MESH
#define DILAY_VIEW_MENU_FREEFORM_MESH

#include "view/tool/menu.hpp"

class ViewMenuFreeformMesh : public ViewToolMenu {
  public:
    ViewMenuFreeformMesh (ViewMainWindow&, const glm::ivec2&);
};

#endif
