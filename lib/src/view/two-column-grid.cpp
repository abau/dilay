/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QButtonGroup>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

struct ViewTwoColumnGrid::Impl
{
  ViewTwoColumnGrid* self;
  QGridLayout*       layout;
  unsigned int       numProperties;

  Impl (ViewTwoColumnGrid* s)
    : self (s)
    , layout (nullptr)
    , numProperties (0)
  {
    this->layout = new QGridLayout;
    this->layout->setColumnStretch (0, 0);
    this->layout->setColumnStretch (1, 1);
    this->self->setLayout (this->layout);
    this->numProperties = 0;
  }

  void add (QWidget& w)
  {
    this->layout->addWidget (&w, this->numProperties, 0, 1, 2);
    this->layout->setRowStretch (this->numProperties, 0);
    this->numProperties++;
  }

  void add (const QString& label, QWidget& w)
  {
    this->add (*new QLabel (label), w);
  }

  void add (QWidget& w1, QWidget& w2)
  {
    this->layout->addWidget (&w1, this->numProperties, 0);
    this->layout->addWidget (&w2, this->numProperties, 1);
    this->layout->setRowStretch (this->numProperties, 0);
    this->numProperties++;
  }

  void addStacked (const QString& l, QWidget& w)
  {
    QVBoxLayout& stack = *new QVBoxLayout;
    QLabel&      label = *new QLabel (l);

    label.setAlignment (Qt::AlignHCenter);
    stack.addWidget (&label);
    stack.addWidget (&w);

    this->layout->addLayout (&stack, this->numProperties, 0, 1, 2);
    this->layout->setRowStretch (this->numProperties, 0);
    this->numProperties++;
  }

  void add (QButtonGroup& group, const std::vector<QString>& labels)
  {
    int id = 0;
    for (const QString& label : labels)
    {
      QRadioButton& button = ViewUtil::radioButton (label);

      group.addButton (&button, id);
      this->add (button);
      id++;
    }
  }

  void addLeft (const QString& label)
  {
    this->add (label, *new QWidget);
  }

  void addCenter (const QString& label)
  {
    this->add (*new QLabel (label));
  }

  void addStretcher ()
  {
    this->add (*new QWidget, *new QWidget);
    this->layout->setRowStretch (this->numProperties, 1);
  }
};

DELEGATE_BIG2_BASE (ViewTwoColumnGrid, (QWidget * p), (this), QWidget, (p))
DELEGATE1 (void, ViewTwoColumnGrid, add, QWidget&)
DELEGATE2 (void, ViewTwoColumnGrid, add, const QString&, QWidget&)
DELEGATE2 (void, ViewTwoColumnGrid, add, QWidget&, QWidget&)
DELEGATE2 (void, ViewTwoColumnGrid, addStacked, const QString&, QWidget&)
DELEGATE2 (void, ViewTwoColumnGrid, add, QButtonGroup&, const std::vector<QString>&)
DELEGATE1 (void, ViewTwoColumnGrid, addLeft, const QString&)
DELEGATE1 (void, ViewTwoColumnGrid, addCenter, const QString&)
DELEGATE (void, ViewTwoColumnGrid, addStretcher)
