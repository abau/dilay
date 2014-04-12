#ifndef DILAY_VIEW_TOOL_OPTIONS
#define DILAY_VIEW_TOOL_OPTIONS

#include <QDialog>
#include "macro.hpp"

class ViewMainWindow;
class QString;
class QMoveEvent;

class ViewToolOptions : public QDialog {
  public:
    DECLARE_BIG3 (ViewToolOptions, ViewMainWindow*);

    template <typename T>
    T* add (const QString& label, T* widget) {
      this->addOption (label, static_cast <QWidget*> (widget) );
      return widget;
    };

    QWidget* addOption (const QString&, QWidget*);

  protected:
    void moveEvent (QMoveEvent*);

  private:
    class Impl;
    Impl* impl;
};

#endif
