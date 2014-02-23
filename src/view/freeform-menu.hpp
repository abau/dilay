#ifndef DILAY_VIEW_FREEFORM_MENU
#define DILAY_VIEW_FREEFORM_MENU

#include <QMenu>
#include "macro.hpp"

class ViewMainWindow;

class ViewFreeformMenu : public QMenu {
    Q_OBJECT
  public:
    DECLARE_BIG3 (ViewFreeformMenu, ViewMainWindow*)

  private:
    class Impl;
    Impl* impl;
};

#endif
