#ifndef DILAY_VIEW_BOTTOM_TOOLBAR
#define DILAY_VIEW_BOTTOM_TOOLBAR

#include <QToolBar>
#include "macro.hpp"

class QSpinBox;
class QString;

class ViewBottomToolbar : public QToolBar {
    Q_OBJECT
  public:
    DECLARE_BIG3 (ViewBottomToolbar)

    QSpinBox* addSpinBox (const QString&, int, int, int);

  private:
    class Impl;
    Impl* impl;
};

#endif
