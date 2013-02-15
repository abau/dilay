#include <QPoint>
#include "maybe.hpp"

#ifndef VIEW_MOUSE_MOVEMENT
#define VIEW_MOUSE_MOVEMENT

class MouseMovement {
  public:
    void  update      (const QPoint& p);
    void  invalidate  ();
    int   dX          () const;
    int   dY          () const;

  private:
    Maybe <QPoint> _oldPos;
    Maybe <QPoint> _newPos;
};

#endif
