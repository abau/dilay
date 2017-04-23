/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_MAIN_WINDOW
#define DILAY_VIEW_MAIN_WINDOW

#include <QMainWindow>
#include "macro.hpp"

class Cache;
class Config;
class QCloseEvent;
class ViewGlWidget;
class ViewInfoPane;
class ViewToolPane;

class ViewMainWindow : public QMainWindow
{
  Q_OBJECT
public:
  DECLARE_BIG2 (ViewMainWindow, Config&, Cache&)

  ViewGlWidget& glWidget ();
  ViewToolPane& toolPane ();
  ViewInfoPane& infoPane ();
  void          update ();

protected:
  void closeEvent (QCloseEvent*);

private:
  IMPLEMENTATION
};
#endif
