#include <QPushButton>
#include <QGridLayout>
#include <QSpinBox>
#include <QLabel>
#include "view/tool-options.hpp"
#include "view/main-window.hpp"
#include "view/vector-edit.hpp"

struct ViewToolOptions::Impl {
  ViewToolOptions* self;
  QVBoxLayout*     vBoxLayout;
  QGridLayout*     gridLayout;
  QPushButton*     cancelButton;
  QPushButton*     applyButton;

  Impl (ViewToolOptions* s, ViewMainWindow* p) 
    : self   (s)
  {
    this->vBoxLayout = new QVBoxLayout;
    this->gridLayout = new QGridLayout;

    this->gridLayout->setColumnStretch (0,0);
    this->gridLayout->setColumnStretch (1,1);
    this->vBoxLayout->addLayout        (this->gridLayout);
    this->self->setParent              (p, this->self->windowFlags ());
    this->self->setLayout              (this->vBoxLayout);
    this->self->move                   (100,100);
    this->self->setAttribute           (Qt::WA_DeleteOnClose);
    this->makeDefaultButtons           ();
  }
  
  void makeDefaultButtons () {
    this->cancelButton = this->pushButton (tr("Cancel"));
    this->applyButton  = this->pushButton (tr("Apply"));

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(this->cancelButton);
    hLayout->addWidget(this->applyButton);

    this->vBoxLayout->addLayout (hLayout);

    QObject::connect ( this->cancelButton, &QPushButton::released
                     , [this] () { this->self->reject (); } );
    QObject::connect ( this->applyButton, &QPushButton::released
                     , [this] () { this->self->accept (); } );
  }

  void addGridRow (QWidget* w1, QWidget* w2) {
    const int r = this->gridLayout->rowCount ();
    this->gridLayout->addWidget (w1, r, 0);
    this->gridLayout->addWidget (w2, r, 1);
  }

  void addGridRow (const QString& label, QWidget* w2) {
    this->addGridRow (new QLabel (label), w2);
  }

  QPushButton* pushButton (const QString& label) {
    QPushButton* button = new QPushButton (label);
    button->setFocusPolicy (Qt::NoFocus);
    return button;
  }

  QSpinBox* spinBox (const QString& label, int min, int value, int max) {
    QSpinBox* spinBox = new QSpinBox;
    spinBox->setRange       (min, max);
    spinBox->setValue       (value);
    spinBox->setFocusPolicy (Qt::NoFocus);

    this->addGridRow (label, spinBox);
    return spinBox;
  }

  ViewVectorEdit* vectorEdit (const QString& label) {
    ViewVectorEdit* vectorEdit = new ViewVectorEdit;
    this->addGridRow (label, vectorEdit);
    return vectorEdit;
  }
};

DELEGATE1_BIG3_SELF (ViewToolOptions, ViewMainWindow*)

DELEGATE4 (QSpinBox*      , ViewToolOptions, spinBox, const QString&, int, int, int)
DELEGATE1 (ViewVectorEdit*, ViewToolOptions, vectorEdit, const QString&)
