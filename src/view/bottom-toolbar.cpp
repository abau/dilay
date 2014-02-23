#include "view/bottom-toolbar.hpp"

struct ViewBottomToolbar :: Impl {
  ViewBottomToolbar* self;

  Impl (ViewBottomToolbar* s) : self (s) {
    this->self->setMovable   (false);
    this->self->setFloatable (false);
  }
}; 

DELEGATE_BIG3_SELF (ViewBottomToolbar)
