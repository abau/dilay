#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include "view/properties.hpp"
#include "view/properties/button.hpp"

struct ViewProperties::Impl {
  ViewProperties* self;
  QGridLayout*    bodyLayout;
  QFrame*         body;

  Impl (ViewProperties* s) : self (s) {
    QVBoxLayout* vBoxLayout = new QVBoxLayout;

    vBoxLayout->setSpacing         (0);
    vBoxLayout->setContentsMargins (0,0,0,0);
    this->makeHeader               (vBoxLayout);
    this->makeBody                 (vBoxLayout);

    this->self->setLayout (vBoxLayout);
  }

  void makeHeader (QBoxLayout* globalLayout) {
    ViewPropertiesButton* button = new ViewPropertiesButton ("Blabla");

    QObject::connect (button, &ViewPropertiesButton::expand,   [this] () { this->body->show (); });
    QObject::connect (button, &ViewPropertiesButton::collapse, [this] () { this->body->hide (); });

    globalLayout->addWidget (button);
  }

  void makeBody (QBoxLayout* globalLayout) {
    this->body       = new QFrame;
    this->bodyLayout = new QGridLayout;

    this->bodyLayout->setSpacing         (0);
    this->bodyLayout->setContentsMargins (0,0,0,0);
    this->body->setLayout                (this->bodyLayout);

    globalLayout->addWidget (this->body);
  }

  QWidget* addWidget (const QString& label, QWidget* widget) {
    const int r = this->bodyLayout->rowCount ();
    this->bodyLayout->addWidget (new QLabel (label), r, 0);
    this->bodyLayout->addWidget (widget            , r, 1);
    return widget;
  }
};

DELEGATE_BIG3_SELF (ViewProperties)
DELEGATE2 (QWidget*, ViewProperties, addWidget, const QString&, QWidget*)
