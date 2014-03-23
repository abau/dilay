#include "view/main-widget.hpp"
#include "view/gl-widget.hpp"
#include "view/properties/widget.hpp"

struct ViewMainWidget :: Impl {
  ViewMainWidget*       self;
  ViewGlWidget*         glWidget;
  ViewPropertiesWidget* properties;

  Impl (ViewMainWidget* s, ViewMainWindow* mW) : self (s) {
    QGLFormat glFormat;
    glFormat.setVersion         (2,1);
    glFormat.setProfile         (QGLFormat::CoreProfile); 
    glFormat.setDepth           (true); 
    glFormat.setDepthBufferSize (24); 
  
    this->glWidget      = new ViewGlWidget         (glFormat, mW);
    this->properties    = new ViewPropertiesWidget ();

    this->self->addWidget (this->properties);
    this->self->addWidget (this->glWidget);
  }

};

DELEGATE1_BIG3_SELF (ViewMainWidget, ViewMainWindow*)
GETTER (ViewGlWidget*        , ViewMainWidget, glWidget)
GETTER (ViewPropertiesWidget*, ViewMainWidget, properties)
