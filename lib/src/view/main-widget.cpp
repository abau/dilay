#include "view/gl-widget.hpp"
#include "view/main-widget.hpp"
#include "view/properties/widget.hpp"

struct ViewMainWidget :: Impl {
  ViewMainWidget*       self;
  ViewGlWidget&         glWidget;
  ViewPropertiesWidget& properties;

  Impl (ViewMainWidget* s, ViewMainWindow& mW, Config& config) 
    : self       (s) 
    , glWidget   (*new ViewGlWidget (mW, config))
    , properties (*new ViewPropertiesWidget ())
  {
    this->self->addWidget (&this->properties);
    this->self->addWidget (&this->glWidget);
  }
};

DELEGATE2_BIG3_SELF (ViewMainWidget, ViewMainWindow&, Config&)
GETTER (ViewGlWidget&        , ViewMainWidget, glWidget)
GETTER (ViewPropertiesWidget&, ViewMainWidget, properties)
