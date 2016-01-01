/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_PROPERTIES
#define DILAY_VIEW_PROPERTIES

#include <QWidget>
#include "macro.hpp"

class QButtonGroup;
class QString;
class QVBoxLayout;

class ViewPropertiesPart {
  public:
    DECLARE_BIG3 (ViewPropertiesPart, QVBoxLayout&, unsigned int)

    void reset      ();
    void add        (QWidget&);
    void add        (const QString&, QWidget&);
    void add        (QWidget&, QWidget&);
    void addStacked (const QString&, QWidget&);
    void add        (QButtonGroup&, const std::vector <QString>&);

  private:
    IMPLEMENTATION
};

class ViewProperties : public QWidget {
  public:
    DECLARE_BIG2 (ViewProperties)

    void                reset  ();
    ViewPropertiesPart& header ();
    ViewPropertiesPart& body   ();
    ViewPropertiesPart& footer ();

  private:
    IMPLEMENTATION
};

#endif
