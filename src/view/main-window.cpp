#include "view/main-window.hpp"
#include "view/main-widget.hpp"
#include "view/top-toolbar.hpp"

struct ViewMainWindow :: Impl {
  ViewMainWindow* self;
  ViewMainWidget* mainWidget;
  ViewTopToolbar* topToolbar;

  Impl (ViewMainWindow* s) : self (s) {
    this->mainWidget = new ViewMainWidget (this->self);
    this->topToolbar = new ViewTopToolbar ();

    this->self->setCentralWidget (this->mainWidget);
    this->self->addToolBar       (Qt::TopToolBarArea, this->topToolbar);
  }

  ViewGlWidget*         glWidget   () { return this->mainWidget->glWidget   (); }
  ViewPropertiesWidget* properties () { return this->mainWidget->properties (); }
};

DELEGATE_BIG3_SELF (ViewMainWindow)
GETTER   (ViewTopToolbar*      , ViewMainWindow, topToolbar)
DELEGATE (ViewGlWidget*        , ViewMainWindow, glWidget)
DELEGATE (ViewPropertiesWidget*, ViewMainWindow, properties)
