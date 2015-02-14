#include <QVBoxLayout>
#include "view/properties.hpp"
#include "view/properties/button.hpp"
#include "view/properties/part.hpp"

struct ViewProperties::Impl {
  ViewProperties*       self;
  ViewPropertiesButton  headerButton;
  QVBoxLayout           layout;
  QWidget               partsWidget;
  QVBoxLayout           partsLayout;
  ViewPropertiesPart    header;
  ViewPropertiesPart    body;
  ViewPropertiesPart    footer;

  Impl (ViewProperties* s) 
    : self          (s) 
    , headerButton  ("?")
    , header        (this->partsLayout, 0)
    , body          (this->partsLayout, 1)
    , footer        (this->partsLayout, 2)
  {
    this->self->setLayout (&this->layout);
    this->layout.setSpacing         (0);
    this->layout.setContentsMargins (0,0,0,0);

    this->partsWidget.setLayout (&this->partsLayout);
    this->partsLayout.setSpacing         (0);
    this->partsLayout.setContentsMargins (0,0,0,0);

    QObject::connect (&this->headerButton, &ViewPropertiesButton::expand, [this] () { 
      this->partsWidget.show ();
    });
    QObject::connect (&this->headerButton, &ViewPropertiesButton::collapse, [this] () { 
      this->partsWidget.hide ();
    });
    this->layout.insertWidget (0, &this->headerButton);
    this->layout.insertWidget (1, &this->partsWidget);
  }

  void label (const QString& label) {
    this->headerButton.setText (label);
  }

  void reset () {
    this->header.reset ();
    this->body  .reset ();
    this->footer.reset ();
  }
};

DELEGATE_BIG2_SELF (ViewProperties)
DELEGATE1 (void, ViewProperties, label, const QString&)
DELEGATE  (void, ViewProperties, reset)
GETTER    (ViewPropertiesPart&, ViewProperties, header)
GETTER    (ViewPropertiesPart&, ViewProperties, body)
GETTER    (ViewPropertiesPart&, ViewProperties, footer)
