#include "view/gl-widget.hpp"
#include "view/main-widget.hpp"
#include "view/properties/widget.hpp"

struct ViewMainWidget :: Impl {
  ViewMainWidget*       self;
  ViewGlWidget&         glWidget;
  ViewPropertiesWidget& properties;

  Impl (ViewMainWidget* s, ViewMainWindow& mW) 
    : self       (s) 
    , glWidget   (*new ViewGlWidget (initFormat (), mW))
    , properties (*new ViewPropertiesWidget ())
  {
    this->self->addWidget (&this->properties);
    this->self->addWidget (&this->glWidget);
  }

  QGLFormat initFormat () {
    QGLFormat glFormat;
    glFormat.setVersion         (2,1);
    glFormat.setProfile         (QGLFormat::CoreProfile); 
    glFormat.setDepth           (true); 
    glFormat.setDepthBufferSize (24); 
    return glFormat;
  }
};

DELEGATE1_BIG3_SELF (ViewMainWidget, ViewMainWindow&)
GETTER (ViewGlWidget&        , ViewMainWidget, glWidget)
GETTER (ViewPropertiesWidget&, ViewMainWidget, properties)
