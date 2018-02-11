/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_INFO_PANE
#define DILAY_VIEW_INFO_PANE

#include <QDockWidget>
#include "macro.hpp"

class ViewGlWidget;
class ViewInfoPaneScene;
class ViewToolTip;

class ViewInfoPane : public QDockWidget
{
public:
  DECLARE_BIG2 (ViewInfoPane, ViewGlWidget&, QWidget* = nullptr)

  ViewInfoPaneScene& scene ();
  void               addToolTip (const ViewToolTip&);
  void               resetToolTip ();
  void               reset ();

private:
  IMPLEMENTATION
};

#endif
