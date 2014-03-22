#include <QGridLayout>
#include <QLabel>
#include "view/property.hpp"

struct ViewProperty::Impl {
  ViewProperty* self;
  QGridLayout*  layout;

  Impl (ViewProperty* s) : self (s) {
    this->layout = new QGridLayout;
    this->self->setLayout (this->layout);
    this->self->setFocusPolicy (Qt::NoFocus);
  }

  QWidget* addWidget (const QString& label, QWidget* widget) {
    const int r = this->layout->rowCount ();
    this->layout->addWidget (new QLabel (label), r, 0);
    this->layout->addWidget (widget            , r, 1);
    return widget;
  }
};

DELEGATE_BIG3_SELF (ViewProperty)
DELEGATE2 (QWidget*, ViewProperty, addWidget, const QString&, QWidget*)
