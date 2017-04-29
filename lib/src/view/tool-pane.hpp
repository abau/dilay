/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_TOOL_PANE
#define DILAY_VIEW_TOOL_PANE

#include <QDockWidget>
#include "macro.hpp"

class ViewGlWidget;
class ViewTwoColumnGrid;

class ViewToolPane : public QDockWidget
{
public:
  DECLARE_BIG2 (ViewToolPane, ViewGlWidget&, QWidget* = nullptr)

  ViewTwoColumnGrid& makeProperties ();
  void               resetProperties ();

private:
  IMPLEMENTATION
};

#endif
