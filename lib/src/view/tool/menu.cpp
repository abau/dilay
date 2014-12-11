#include <glm/glm.hpp>
#include "view/tool/menu-parameters.hpp"
#include "view/tool/menu.hpp"

struct ViewToolMenu::Impl {
  ViewToolMenu*    self;
  const glm::ivec2 menuPosition;

  Impl (ViewToolMenu* s, const glm::ivec2& p)
    : self         (s)
    , menuPosition (p)
  {}

  ViewToolMenuParameters menuParameters (const QString& label) const {
    return ViewToolMenuParameters (this->menuPosition, label);
  }

  void addSeparator () {
    this->self->QMenu::addSeparator ();
  }
};

DELEGATE1_BIG3_SELF (ViewToolMenu, const glm::ivec2&)
DELEGATE1_CONST (ViewToolMenuParameters, ViewToolMenu, menuParameters, const QString&)
DELEGATE        (void                  , ViewToolMenu, addSeparator)
