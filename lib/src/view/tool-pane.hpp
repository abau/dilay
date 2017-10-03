/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_TOOL_PANE
#define DILAY_VIEW_TOOL_PANE

#include <QDockWidget>
#include "macro.hpp"

class QPushButton;
class ViewGlWidget;
class ViewTwoColumnGrid;

enum class ViewToolPaneSelection
{
  Sculpt,
  Sketch
};

class ViewToolPane : public QDockWidget
{
public:
  DECLARE_BIG2 (ViewToolPane, ViewGlWidget&, QWidget* = nullptr)

  ViewTwoColumnGrid&    properties ();
  void                  forceWidth ();
  ViewToolPaneSelection selection () const;
  QPushButton&          button (const char*);

private:
  IMPLEMENTATION
};

#endif
