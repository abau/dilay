/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QButtonGroup>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>
#include "view/properties.hpp"
#include "view/util.hpp"

struct ViewPropertiesPart::Impl {
  QVBoxLayout&       parentLayout;
  const unsigned int index;
  QWidget*           widget;
  QGridLayout*       layout;
  unsigned int       numProperties;

  Impl (QVBoxLayout& p, unsigned int i)
    : parentLayout  (p)
    , index         (i)
    , widget        (nullptr)
    , layout        (nullptr)
    , numProperties (0)
  {
    this->reset ();
  }

  void reset () {
    if (this->widget) {
      assert (this->layout);
      this->parentLayout.removeWidget (this->widget);
      delete this->widget;
    }
    this->layout = new QGridLayout;
    this->widget = new QWidget;
    this->widget->setLayout (this->layout);
    this->widget->layout ()->setContentsMargins (0,0,0,0);

    this->numProperties = 0;

    this->parentLayout.insertWidget (this->index, this->widget);
  }

  void add (QWidget& w) {
    this->layout->addWidget (&w, this->numProperties, 0, 1, 2);
    this->numProperties++;
  }

  void add (const QString& label, QWidget& w) {
    this->add (*new QLabel (label), w);
  }

  void add (QWidget& w1, QWidget& w2) {
    this->layout->addWidget (&w1, this->numProperties, 0);
    this->layout->addWidget (&w2, this->numProperties, 1);
    this->numProperties++;
  }

  void addStacked (const QString& l, QWidget& w) {
    QVBoxLayout& stack = *new QVBoxLayout;
    QLabel&      label = *new QLabel (l);

    label.setAlignment (Qt::AlignHCenter);
    stack.addWidget (&label);
    stack.addWidget (&w);

    this->layout->addLayout (&stack, this->numProperties, 0, 1, 2);
    this->numProperties++;
  }

  void add (QButtonGroup& group, const std::vector <QString>& labels) {
    int id = 0;
    for (const QString& label : labels) {
      QRadioButton& button = ViewUtil::radioButton (label);

      group.addButton (&button, id);
      this->add       (button);
      id++;
    }
  }
};

DELEGATE2_BIG3 (ViewPropertiesPart, QVBoxLayout&, unsigned int)
DELEGATE  (void, ViewPropertiesPart, reset)
DELEGATE1 (void, ViewPropertiesPart, add, QWidget&)
DELEGATE2 (void, ViewPropertiesPart, add, const QString&, QWidget&)
DELEGATE2 (void, ViewPropertiesPart, add, QWidget&, QWidget&)
DELEGATE2 (void, ViewPropertiesPart, addStacked, const QString&, QWidget&)
DELEGATE2 (void, ViewPropertiesPart, add, QButtonGroup&, const std::vector <QString>&)

struct ViewProperties::Impl {
  ViewProperties*    self;
  QVBoxLayout        layout;
  QWidget            partsWidget;
  QVBoxLayout        partsLayout;
  ViewPropertiesPart header;
  ViewPropertiesPart body;
  ViewPropertiesPart footer;

  Impl (ViewProperties* s) 
    : self          (s) 
    , header        (this->partsLayout, 0)
    , body          (this->partsLayout, 1)
    , footer        (this->partsLayout, 2)
  {
    this->self->setLayout (&this->layout);

    this->partsWidget.setLayout (&this->partsLayout);
    this->partsLayout.setSpacing         (0);
    this->partsLayout.setContentsMargins (0,0,0,0);

    this->layout.insertWidget (0, &this->partsWidget);
  }

  void reset () {
    this->header.reset ();
    this->body  .reset ();
    this->footer.reset ();
  }
};

DELEGATE_BIG2_SELF (ViewProperties)
DELEGATE (void, ViewProperties, reset)
GETTER   (ViewPropertiesPart&, ViewProperties, header)
GETTER   (ViewPropertiesPart&, ViewProperties, body)
GETTER   (ViewPropertiesPart&, ViewProperties, footer)
