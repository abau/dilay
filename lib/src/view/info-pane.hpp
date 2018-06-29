/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_INFO_PANE
#define DILAY_VIEW_INFO_PANE

#include <QDockWidget>
#include "macro.hpp"

class ViewInfoPaneScene;
class ViewMainWindow;
class ViewToolTip;

class ViewInfoPane : public QDockWidget
{
public:
  DECLARE_BIG2 (ViewInfoPane, ViewMainWindow&, QWidget* = nullptr)

  ViewInfoPaneScene& scene ();
  void               addToolTip (const ViewToolTip&);
  void               resetToolTip ();
  void               reset ();

private:
  IMPLEMENTATION
};

#endif
