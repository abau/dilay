#include <QButtonGroup>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>
#include "view/properties/part.hpp"
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
    this->widget->layout ()->setSpacing         (3);
    this->widget->layout ()->setContentsMargins (11,3,0,3);

    this->numProperties = 0;

    this->parentLayout.insertWidget (this->index, this->widget);
  }

  void add (QWidget& w) {
    this->layout->addWidget (&w, this->numProperties, 0, 1, 2);
    this->numProperties++;
  }

  void add(const QString& label, QWidget& w) {
    this->layout->addWidget (new QLabel (label), this->numProperties, 0);
    this->layout->addWidget (&w                , this->numProperties, 1);
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
DELEGATE2 (void, ViewPropertiesPart, add, QButtonGroup&, const std::vector <QString>&)
