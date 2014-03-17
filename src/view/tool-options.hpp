#ifndef DILAY_VIEW_TOOL_OPTIONS
#define DILAY_VIEW_TOOL_OPTIONS

#include <QDialog>
#include "macro.hpp"

class ViewMainWindow;
class QPushButton;
class QSpinBox;
class QString;
class ViewVectorEdit;
class QMoveEvent;

class ViewToolOptions : public QDialog {
  public:
    DECLARE_BIG3 (ViewToolOptions, ViewMainWindow*);

    QSpinBox*       spinBox      (const QString&, int, int, int);
    ViewVectorEdit* vectorEdit   (const QString&);

  protected:
    void            moveEvent    (QMoveEvent*);

  private:
    class Impl;
    Impl* impl;
};

#endif
