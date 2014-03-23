#include <QVBoxLayout>
#include "view/properties/widget.hpp"
#include "view/properties/camera.hpp"
#include "view/properties/movement.hpp"
#include "view/properties/selection.hpp"

struct ViewPropertiesWidget::Impl {
  ViewPropertiesWidget* self;

  Impl (ViewPropertiesWidget* s) : self (s) {
    QVBoxLayout* layout = new QVBoxLayout;

    layout->addWidget  (new ViewPropertiesSelection ());
    layout->addStretch (0);
    layout->addWidget  (new ViewPropertiesCamera    ());
    layout->addStretch (0);
    layout->addWidget  (new ViewPropertiesMovement  ());
    layout->addStretch (1);

    this->self->setLayout (layout);
  }
};

DELEGATE_BIG3_SELF (ViewPropertiesWidget)
