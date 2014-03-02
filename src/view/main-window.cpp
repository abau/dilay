#include "view/main-window.hpp"
#include "view/gl-widget.hpp"
#include "view/top-toolbar.hpp"

struct ViewMainWindow :: Impl {
  ViewMainWindow*    self;
  ViewTopToolbar*    topToolbar;
  ViewGlWidget*      glWidget;

  Impl (ViewMainWindow* s) : self (s) {
    QGLFormat glFormat;
    glFormat.setVersion         (2,1);
    glFormat.setProfile         (QGLFormat::CoreProfile); 
    glFormat.setDepth           (true); 
    glFormat.setDepthBufferSize (24); 
    this->topToolbar = new ViewTopToolbar ();
    this->glWidget   = new ViewGlWidget   (glFormat, this->self);

    this->self->setFocusPolicy   (Qt::NoFocus);
    this->self->setCentralWidget (this->glWidget);
    this->self->addToolBar       (Qt::TopToolBarArea, this->topToolbar);
  }
};

DELEGATE_BIG3_SELF (ViewMainWindow)
GETTER (ViewTopToolbar*, ViewMainWindow, topToolbar)
GETTER (ViewGlWidget*  , ViewMainWindow, glWidget)
