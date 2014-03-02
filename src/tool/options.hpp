#ifndef DILAY_TOOL_OPTIONS
#define DILAY_TOOL_OPTIONS

#include <QDialog>
#include "macro.hpp"

class ViewMainWindow;
class QPushButton;
class QSpinBox;
class QString;

class ToolOptions : public QDialog {
  public:
    DECLARE_BIG3 (ToolOptions, ViewMainWindow*);

    QPushButton* cancelButton ();
    QPushButton* applyButton  ();
    QSpinBox*    spinBox      (const QString&, int, int, int);

  private:
    class Impl;
    Impl* impl;
};

#endif
