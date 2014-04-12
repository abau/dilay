#include "view/main-window.hpp"
#include "view/main-widget.hpp"

struct ViewMainWindow :: Impl {
  ViewMainWindow* self;
  ViewMainWidget* mainWidget;

  Impl (ViewMainWindow* s) : self (s) {
    this->mainWidget = new ViewMainWidget (this->self);

    this->self->setCentralWidget (this->mainWidget);
  }

  ViewGlWidget*         glWidget   () { return this->mainWidget->glWidget   (); }
  ViewPropertiesWidget* properties () { return this->mainWidget->properties (); }
};

DELEGATE_BIG3_SELF (ViewMainWindow)
DELEGATE (ViewGlWidget*        , ViewMainWindow, glWidget)
DELEGATE (ViewPropertiesWidget*, ViewMainWindow, properties)
