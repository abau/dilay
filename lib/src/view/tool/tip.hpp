#ifndef DILAY_VIEW_TOOL_TIP
#define DILAY_VIEW_TOOL_TIP

#include "macro.hpp"

class QString;

class ViewToolTip {
  public:
    DECLARE_BIG4COPY (ViewToolTip)

    enum class MouseEvent { Left, Middle, Wheel, Right };
    enum class Modifier   { None, Ctrl, Shift, Alt };

    QString toString () const;

    void add   (MouseEvent, Modifier, const QString&);
    void add   (MouseEvent, const QString&);
    void reset ();

  private:
    IMPLEMENTATION
};

#endif
