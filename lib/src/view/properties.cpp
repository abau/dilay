#include <QButtonGroup>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>
#include "view/properties.hpp"
#include "view/properties/button.hpp"
#include "view/util.hpp"

struct ViewProperties::Impl {
  ViewProperties*       self;
  ViewPropertiesButton& headerButton;
  QWidget*              body;
  unsigned int          numProperties;
  QWidget*              footer;

  Impl (ViewProperties* s) 
    : self          (s) 
    , headerButton  (*new ViewPropertiesButton ("?"))
    , body          (nullptr)
    , numProperties (0)
    , footer        (nullptr)
  {
    this->self->setLayout (new QVBoxLayout);
    this->self->layout ()->setSpacing         (0);
    this->self->layout ()->setContentsMargins (0,0,0,0);

    this->setupHeader ();
    this->resetBody   ();
  }

  QVBoxLayout* layout () const {
    return dynamic_cast <QVBoxLayout*> (this->self->layout ());
  }

  QGridLayout* bodyLayout () const {
    return dynamic_cast <QGridLayout*> (this->body->layout ());
  }

  void setupHeader () {
    QObject::connect (&this->headerButton, &ViewPropertiesButton::expand,   [this] () { 
      this->body->show (); 
      if (this->footer) {
        this->footer->show ();
      }
    });
    QObject::connect (&this->headerButton, &ViewPropertiesButton::collapse, [this] () { 
      this->body->hide (); 
      if (this->footer) {
        this->footer->hide ();
      }
    });
    this->layout ()->insertWidget (0, &this->headerButton);
  }

  void resetBody () {
    if (this->body) {
      this->self->layout ()->removeWidget (this->body);
      delete this->body;
    }
    this->body = new QWidget;
    this->body->setLayout (new QGridLayout);
    this->body->layout ()->setSpacing         (3);
    this->body->layout ()->setContentsMargins (11,3,0,3);

    this->layout ()->insertWidget (1, this->body);
  }

  void resetFooter () {
    if (this->footer) {
      this->layout ()->removeWidget (this->footer);
      delete this->footer;
      this->footer = nullptr;
    }
  }

  void setLabel (const QString& label) {
    this->headerButton.setText (label);
  }

  void addWidget (const QString& label, QWidget& widget) {
    assert (this->body);
    this->bodyLayout ()->addWidget (new QLabel (label), this->numProperties, 0);
    this->bodyLayout ()->addWidget (&widget           , this->numProperties, 1);
    this->numProperties++;
  }

  void addWidget (QWidget& widget) {
    assert (this->body);
    this->bodyLayout ()->addWidget (&widget, this->numProperties, 0, 1, 2);
    this->numProperties++;
  }

  void addRadioButtons (QButtonGroup& group, const std::vector <QString>& labels) {
    int id = 0;
    for (const QString& label : labels) {
      QRadioButton& button = ViewUtil::radioButton (label);

      group.addButton (&button, id);
      this->addWidget (button);
      id++;
    }
  }

  void setFooter (QWidget& widget) {
    this->resetFooter ();
    this->footer = &widget;
    this->layout ()->insertWidget (2, this->footer);
  }

  void resetWidgets () {
    this->resetBody   ();
    this->resetFooter ();
  }
};

DELEGATE_BIG3_SELF (ViewProperties)
DELEGATE1 (void, ViewProperties, setLabel, const QString&)
DELEGATE2 (void, ViewProperties, addWidget, const QString&, QWidget&)
DELEGATE1 (void, ViewProperties, addWidget, QWidget&)
DELEGATE2 (void, ViewProperties, addRadioButtons, QButtonGroup&, const std::vector <QString>&)
DELEGATE1 (void, ViewProperties, setFooter, QWidget&)
DELEGATE  (void, ViewProperties, resetWidgets)
