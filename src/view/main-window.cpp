#include "view/main-window.hpp"
#include "view/gl-widget.hpp"

struct ViewMainWindow :: Impl {
  ViewMainWindow* self;
  ViewGlWidget*   glWidget;

  Impl (ViewMainWindow* s) : self (s) {
    QGLFormat glFormat;
    glFormat.setVersion         (2,1);
    glFormat.setProfile         (QGLFormat::CoreProfile); 
    glFormat.setDepth           (true); 
    glFormat.setDepthBufferSize (24); 
    this->glWidget = new ViewGlWidget (glFormat);

    this->self->setFocusPolicy   (Qt::NoFocus);
    this->self->setCentralWidget (this->glWidget);
  }
};

DELEGATE_WIDGET_BIG6 (ViewMainWindow)
