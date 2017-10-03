/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_TWO_COLUMN_GRID
#define DILAY_VIEW_TWO_COLUMN_GRID

#include <QWidget>
#include <vector>
#include "macro.hpp"

class QButtonGroup;
class QString;

class ViewTwoColumnGrid : public QWidget
{
public:
  DECLARE_BIG2 (ViewTwoColumnGrid, QWidget* = nullptr)

  unsigned int numRows () const;
  void         setEqualColumnStretch ();

  void add (QWidget&);
  void add (const QString&, const QString&);
  void add (const QString&, QWidget&);
  void add (QWidget&, QWidget&);
  void add (QButtonGroup&);
  void addStacked (const QString&, QWidget&);
  void addStacked (const QString&, QButtonGroup&);
  void addLeft (const QString&);
  void addCenter (const QString&);
  void addStretcher ();
  void addSeparator ();
  void reset ();

private:
  IMPLEMENTATION
};

#endif
