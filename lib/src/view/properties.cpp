#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include "view/properties.hpp"
#include "view/properties/button.hpp"

struct ViewProperties::Impl {
  ViewProperties*       self;
  ViewPropertiesButton& headerButton;
  QGridLayout&          bodyLayout;
  QFrame&               body;

  Impl (ViewProperties* s) 
    : self         (s) 
    , headerButton (*new ViewPropertiesButton ("?"))
    , bodyLayout   (*new QGridLayout)
    , body         (*new QFrame)
  {
    QVBoxLayout* vBoxLayout = new QVBoxLayout;

    vBoxLayout->setSpacing         (0);
    vBoxLayout->setContentsMargins (0,0,0,0);
    this->makeHeader               (vBoxLayout);
    this->makeBody                 (vBoxLayout);

    this->self->setLayout (vBoxLayout);
  }

  void makeHeader (QBoxLayout* globalLayout) {
    QObject::connect (&this->headerButton, &ViewPropertiesButton::expand,   [this] () { 
      this->body.show (); 
    });
    QObject::connect (&this->headerButton, &ViewPropertiesButton::collapse, [this] () { 
      this->body.hide (); 
    });
    globalLayout->addWidget (&this->headerButton);
  }

  void makeBody (QBoxLayout* globalLayout) {
    this->bodyLayout.setSpacing         (0);
    this->bodyLayout.setContentsMargins (11,0,0,0);
    this->body.setLayout                (&this->bodyLayout);

    globalLayout->addWidget (&this->body);
  }

  void setLabel (const QString& label) {
    this->headerButton.setText (label);
  }

  QWidget& addWidget (const QString& label, QWidget& widget) {
    const int r = this->bodyLayout.rowCount ();
    this->bodyLayout.addWidget (new QLabel (label), r, 0);
    this->bodyLayout.addWidget (&widget           , r, 1);
    return widget;
  }

  QWidget& addWidget (QWidget& widget) {
    const int r = this->bodyLayout.rowCount ();
    this->bodyLayout.addWidget (&widget, r, 0, 1, 2);
    return widget;
  }
};

DELEGATE_BIG3_SELF (ViewProperties)
DELEGATE1 (void    , ViewProperties, setLabel, const QString&)
DELEGATE2 (QWidget&, ViewProperties, addWidget, const QString&, QWidget&)
DELEGATE1 (QWidget&, ViewProperties, addWidget, QWidget&)
