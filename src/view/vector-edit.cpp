#include <QHBoxLayout>
#include <QLineEdit>
#include "view/vector-edit.hpp"

struct ViewVectorEdit::Impl {
  ViewVectorEdit* self;

  Impl (ViewVectorEdit* s) : self (s) { 
    QHBoxLayout* layout = new QHBoxLayout;
    QLineEdit*   x      = new QLineEdit;
    QLineEdit*   y      = new QLineEdit;
    QLineEdit*   z      = new QLineEdit;

    layout->setSpacing (0);
    layout->addWidget  (x);
    layout->addWidget  (y);
    layout->addWidget  (z);

    this->self->setLayout (layout);
  };
};

ViewVectorEdit :: ViewVectorEdit (QWidget* p) : QWidget (p) {
  this->impl = new Impl (this);
}
ViewVectorEdit :: ViewVectorEdit () : ViewVectorEdit (nullptr) {}

DELEGATE_BIG3_WITHOUT_CONSTRUCTOR (ViewVectorEdit) 
