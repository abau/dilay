#ifndef DILAY_VIEW_MAIN_WINDOW
#define DILAY_VIEW_MAIN_WINDOW

#include <QMainWindow>
#include "macro.hpp"

class ViewTopToolbar;
class ViewBottomToolbar;

class ViewMainWindow : public QMainWindow {
    Q_OBJECT
  public:
    DECLARE_BIG3 (ViewMainWindow)

    ViewTopToolbar*    topToolbar    ();
    ViewBottomToolbar* bottomToolbar ();

  private:
    class Impl;
    Impl* impl;
};
#endif
