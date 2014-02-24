#include <QSpinBox>
#include "view/bottom-toolbar.hpp"

struct ViewBottomToolbar :: Impl {
  ViewBottomToolbar* self;

  Impl (ViewBottomToolbar* s) : self (s) {
    this->self->setMovable   (false);
    this->self->setFloatable (false);
  }

  QSpinBox* addSpinBox ( const QString& label, int min, int value, int max) {
    QSpinBox* spinBox = new QSpinBox;
    spinBox->setRange       (min, max);
    spinBox->setValue       (value);
    spinBox->setSuffix      (" " + label);
    spinBox->setFocusPolicy (Qt::NoFocus);
    this->self->addWidget   (spinBox);
    return spinBox;
  }
}; 

DELEGATE_BIG3_SELF (ViewBottomToolbar)
DELEGATE4 (QSpinBox*, ViewBottomToolbar, addSpinBox, const QString&, int, int, int)
