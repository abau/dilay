#ifndef DILAY_VIEW_TOOL_PARAMETERS
#define DILAY_VIEW_TOOL_PARAMETERS

#include <QDialog>
#include "macro.hpp"

class ViewMainWindow;
class QString;
class QMoveEvent;

class ViewToolParameters : public QDialog {
  public:
    DECLARE_BIG3 (ViewToolParameters, ViewMainWindow&);

    ViewToolParameters (ViewMainWindow&, bool);

    enum Result : int { Apply, ApplyAndClose, Cancel };

    template <typename T>
    T& add (const QString& label, T& widget) {
      this->addParameter (label, static_cast <QWidget&> (widget) );
      return widget;
    };

    QWidget& addParameter (const QString&, QWidget&);

  protected:
    void moveEvent (QMoveEvent*);

  private:
    IMPLEMENTATION
};

#endif
