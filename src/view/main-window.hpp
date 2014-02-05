#ifndef DILAY_VIEW_MAIN_WINDOW
#define DILAY_VIEW_MAIN_WINDOW

#include <QMainWindow>
#include "macro.hpp"

class ViewMainWindow : public QMainWindow {
    Q_OBJECT
  public:
    DECLARE_WIDGET_BIG6 (ViewMainWindow)

  private:
    class Impl;
    Impl* impl;

};
#endif
