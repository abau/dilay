#include <glm/glm.hpp>
#include "view/tool-menu-parameters.hpp"
#include "view/tool-menu.hpp"

struct ViewToolMenu::Impl {
  ViewToolMenu*    self;
  ViewMainWindow&  mainWindow;
  const glm::ivec2 menuPosition;

  Impl (ViewToolMenu* s, ViewMainWindow& mW, const glm::ivec2& p)
    : self         (s)
    , mainWindow   (mW)
    , menuPosition (p)
  {}

  ViewToolMenuParameters menuParameters () const {
    return ViewToolMenuParameters (this->mainWindow, this->menuPosition);
  }
};

DELEGATE2_BIG3_SELF (ViewToolMenu, ViewMainWindow&, const glm::ivec2&)
DELEGATE_CONST (ViewToolMenuParameters, ViewToolMenu, menuParameters)
