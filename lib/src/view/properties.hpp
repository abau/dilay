/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_PROPERTIES
#define DILAY_VIEW_PROPERTIES

#include "view/two-column-grid.hpp"

class ViewProperties : public QWidget {
  public:
    DECLARE_BIG2 (ViewProperties)

    void               reset  ();
    ViewTwoColumnGrid& header ();
    ViewTwoColumnGrid& body   ();
    ViewTwoColumnGrid& footer ();

  private:
    IMPLEMENTATION
};

#endif
