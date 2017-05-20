/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
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
  QGridLayout&       layout;
  unsigned int       numRows;

  Impl (ViewTwoColumnGrid* s)
    : self (s)
    , layout (*new QGridLayout (this->self))
    , numRows (0)
  {
    this->layout.setColumnStretch (0, 0);
    this->layout.setColumnStretch (1, 1);
  }

  void setEqualColumnStretch ()
  {
    this->layout.setColumnStretch (0, 1);
    this->layout.setColumnStretch (1, 1);
  }

  void add (QWidget& w)
  {
    this->layout.addWidget (&w, this->numRows, 0, 1, 2);
    this->layout.setRowStretch (this->numRows, 0);
    this->numRows++;
  }

  void add (const QString& labelLeft, const QString& labelRight)
  {
    this->add (*new QLabel (labelLeft), *new QLabel (labelRight));
  }

  void add (const QString& label, QWidget& w) { this->add (*new QLabel (label), w); }

  void add (QWidget& w1, QWidget& w2)
  {
    this->layout.addWidget (&w1, this->numRows, 0);
    this->layout.addWidget (&w2, this->numRows, 1);
    this->layout.setRowStretch (this->numRows, 0);
    this->numRows++;
  }

  void addStacked (const QString& l, QWidget& w)
  {
    QVBoxLayout& stack = *new QVBoxLayout;
    QLabel&      label = *new QLabel (l);

    label.setAlignment (Qt::AlignHCenter);
    stack.addWidget (&label);
    stack.addWidget (&w);

    this->layout.addLayout (&stack, this->numRows, 0, 1, 2);
    this->layout.setRowStretch (this->numRows, 0);
    this->numRows++;
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

  void addLeft (const QString& label) { this->add (label, *new QWidget); }

  void addCenter (const QString& label) { this->add (*new QLabel (label)); }

  void addStretcher ()
  {
    this->add (*new QWidget, *new QWidget);
    this->layout.setRowStretch (this->numRows, 1);
  }

  void addSeparator () { this->add (ViewUtil::horizontalLine ()); }

  void reset ()
  {
    QWidget* widget = nullptr;
    while ((widget = this->self->findChild<QWidget*> ()))
    {
      delete widget;
    }
    this->numRows = 0;
  }
};

DELEGATE_BIG2_BASE (ViewTwoColumnGrid, (QWidget * p), (this), QWidget, (p))
GETTER_CONST (unsigned int, ViewTwoColumnGrid, numRows)
DELEGATE (void, ViewTwoColumnGrid, setEqualColumnStretch)
DELEGATE1 (void, ViewTwoColumnGrid, add, QWidget&)
DELEGATE2 (void, ViewTwoColumnGrid, add, const QString&, const QString&)
DELEGATE2 (void, ViewTwoColumnGrid, add, const QString&, QWidget&)
DELEGATE2 (void, ViewTwoColumnGrid, add, QWidget&, QWidget&)
DELEGATE2 (void, ViewTwoColumnGrid, addStacked, const QString&, QWidget&)
DELEGATE2 (void, ViewTwoColumnGrid, add, QButtonGroup&, const std::vector<QString>&)
DELEGATE1 (void, ViewTwoColumnGrid, addLeft, const QString&)
DELEGATE1 (void, ViewTwoColumnGrid, addCenter, const QString&)
DELEGATE (void, ViewTwoColumnGrid, addStretcher)
DELEGATE (void, ViewTwoColumnGrid, addSeparator)
DELEGATE (void, ViewTwoColumnGrid, reset)
