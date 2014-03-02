#ifndef DILAY_TOOL_OPTIONS
#define DILAY_TOOL_OPTIONS

#include <QDialog>
#include "macro.hpp"

class ViewMainWindow;
class QPushButton;
class QSpinBox;
class QString;
class ViewVectorEdit;

class ToolOptions : public QDialog {
  public:
    DECLARE_BIG3 (ToolOptions, ViewMainWindow*);

    QSpinBox*       spinBox      (const QString&, int, int, int);
    ViewVectorEdit* vectorEdit   (const QString&);

  private:
    class Impl;
    Impl* impl;
};

#endif
