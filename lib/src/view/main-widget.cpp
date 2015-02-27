#include "view/gl-widget.hpp"
#include "view/main-widget.hpp"
#include "view/properties/widget.hpp"

struct ViewMainWidget :: Impl {
  ViewMainWidget*      self;
  ViewGlWidget         glWidget;
  ViewPropertiesWidget properties;

  Impl (ViewMainWidget* s, ViewMainWindow& mW, Config& config, Cache& cache) 
    : self       (s) 
    , glWidget   (mW, config, cache)
  {
    this->self->addWidget (&this->properties);
    this->self->addWidget (&this->glWidget);
  }
};

DELEGATE3_BIG2_SELF (ViewMainWidget, ViewMainWindow&, Config&, Cache&)
GETTER (ViewGlWidget&        , ViewMainWidget, glWidget)
GETTER (ViewPropertiesWidget&, ViewMainWidget, properties)
