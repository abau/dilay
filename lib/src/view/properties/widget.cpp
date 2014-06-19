#include <QVBoxLayout>
#include "view/properties/widget.hpp"
#include "view/properties/selection.hpp"

struct ViewPropertiesWidget::Impl {
  ViewPropertiesWidget*    self;
  ViewPropertiesSelection& selection;

  Impl (ViewPropertiesWidget* s) 
    : self      (s) 
    , selection (*new ViewPropertiesSelection ())
  {
    QVBoxLayout* layout = new QVBoxLayout;

    layout->setSpacing (0);
    layout->addWidget  (&this->selection);
    // layout->setSpacing (0);
    // ...
    layout->addStretch (1);

    this->self->setLayout (layout);
  }
};

DELEGATE_BIG3_SELF (ViewPropertiesWidget)
GETTER (ViewPropertiesSelection&, ViewPropertiesWidget, selection)
