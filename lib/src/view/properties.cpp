/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QVBoxLayout>
#include "view/properties.hpp"

struct ViewProperties::Impl {
  ViewProperties*    self;
  QVBoxLayout        layout;
  ViewTwoColumnGrid* _header;
  ViewTwoColumnGrid* _body;
  ViewTwoColumnGrid* _footer;

  Impl (ViewProperties* s) 
    : self    (s) 
    , _header (nullptr)
    , _body   (nullptr)
    , _footer (nullptr)
  {
    this->self->setLayout (&this->layout);
    this->reset ();
  }

  ViewTwoColumnGrid& header () {
    assert (this->_header);
    return *this->_header;
  }

  ViewTwoColumnGrid& body () {
    assert (this->_body);
    return *this->_body;
  }

  ViewTwoColumnGrid& footer () {
    assert (this->_footer);
    return *this->_footer;
  }

  void resetTwoColumnGrid (ViewTwoColumnGrid*& grid) {
    if (grid) {
      this->layout.removeWidget (grid);
      delete grid;
    }
    grid = new ViewTwoColumnGrid;
    this->layout.addWidget (grid);
  }

  void reset () {
    this->resetTwoColumnGrid (this->_header);
    this->resetTwoColumnGrid (this->_body);
    this->resetTwoColumnGrid (this->_footer);
  }
};

DELEGATE_BIG2_SELF (ViewProperties)
DELEGATE (ViewTwoColumnGrid&, ViewProperties, header)
DELEGATE (ViewTwoColumnGrid&, ViewProperties, body)
DELEGATE (ViewTwoColumnGrid&, ViewProperties, footer)
DELEGATE (void, ViewProperties, reset)
