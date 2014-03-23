#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>
#include "view/properties.hpp"

struct ViewProperties::Impl {
  ViewProperties* self;
  QGridLayout*    bodyLayout;
  QFrame*         body;

  Impl (ViewProperties* s) : self (s) {
    QVBoxLayout* vBoxLayout = new QVBoxLayout;

    this->makeHeader (vBoxLayout);
    this->makeBody   (vBoxLayout);

    this->self->setLayout (vBoxLayout);
  }

  void makeHeader (QBoxLayout* globalLayout) {
    QHBoxLayout* layout = new QHBoxLayout;

    QToolButton* button = new QToolButton;
    button->setArrowType (Qt::DownArrow);
    button->setCheckable (true);
    button->setChecked   (true);

    QObject::connect (button, &QToolButton::toggled, [this,button] (bool checked) {
        if (checked) {
          button->setArrowType (Qt::DownArrow);
          this->body->show ();
        }
        else {
          button->setArrowType (Qt::RightArrow);
          this->body->hide ();
        }
      }
    );

    layout->addWidget (button);
    layout->addWidget (new QLabel ("Foo"));

    globalLayout->addLayout (layout);
  }

  void makeBody (QBoxLayout* globalLayout) {
    this->body       = new QFrame;
    this->bodyLayout = new QGridLayout;

    this->body->setLayout (this->bodyLayout);

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
