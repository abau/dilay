/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_MAIN_WINDOW
#define DILAY_VIEW_MAIN_WINDOW

#include <QMainWindow>
#include "macro.hpp"

class Cache;
class Config;
class QCloseEvent;
class ViewMainWidget;
class ViewToolTip;

class ViewMainWindow : public QMainWindow {
    Q_OBJECT
  public:
    DECLARE_BIG2 (ViewMainWindow, Config&, Cache&)

    ViewMainWidget& mainWidget         ();
    void            showMessage        (const QString&);
    void            showToolTip        (const ViewToolTip&);
    void            showDefaultToolTip ();
    void            showNumFaces       (unsigned int);
    void            update             ();

  protected:
    void            closeEvent         (QCloseEvent*);

  private:
    IMPLEMENTATION
};
#endif
