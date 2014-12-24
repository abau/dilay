#include <glm/glm.hpp>
#include "view/tool/menu-parameters.hpp"
#include "view/tool/menu.hpp"

struct ViewToolMenu::Impl {
  ViewToolMenu*    self;
  State&           state;
  const glm::ivec2 menuPosition;

  Impl (ViewToolMenu* s, State& st, const glm::ivec2& p)
    : self         (s)
    , state        (st)
    , menuPosition (p)
  {}

  ViewToolMenuParameters menuParameters (const QString& label) const {
    return ViewToolMenuParameters (this->state, this->menuPosition, label);
  }

  void addSeparator () {
    this->self->QMenu::addSeparator ();
  }
};

DELEGATE2_BIG3_SELF (ViewToolMenu, State&, const glm::ivec2&)
GETTER_CONST    (State&                , ViewToolMenu, state)
DELEGATE1_CONST (ViewToolMenuParameters, ViewToolMenu, menuParameters, const QString&)
DELEGATE        (void                  , ViewToolMenu, addSeparator)
