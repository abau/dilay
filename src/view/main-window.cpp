#include "view/main-window.hpp"
#include "view/gl-widget.hpp"
#include "view/toolbar.hpp"

struct ViewMainWindow :: Impl {
  ViewMainWindow* self;
  ViewGlWidget*   glWidget;

  Impl (ViewMainWindow* s) : self (s) {
    QGLFormat glFormat;
    glFormat.setVersion         (2,1);
    glFormat.setProfile         (QGLFormat::CoreProfile); 
    glFormat.setDepth           (true); 
    glFormat.setDepthBufferSize (24); 
    ViewToolbar* toolbar = new ViewToolbar  (nullptr);
    this->glWidget = new ViewGlWidget (glFormat, toolbar);

    this->self->setFocusPolicy   (Qt::NoFocus);
    this->self->setCentralWidget (this->glWidget);
    this->self->addToolBar       (toolbar);
  }
};

DELEGATE_WIDGET_BIG6 (ViewMainWindow)
