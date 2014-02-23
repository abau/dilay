#ifndef DILAY_VIEW_BOTTOM_TOOLBAR
#define DILAY_VIEW_BOTTOM_TOOLBAR

#include <QToolBar>
#include "macro.hpp"

class ViewBottomToolbar : public QToolBar {
    Q_OBJECT
  public:
    DECLARE_BIG3 (ViewBottomToolbar)

  private:
    class Impl;
    Impl* impl;
};

#endif
