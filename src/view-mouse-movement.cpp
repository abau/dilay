#include <QPoint>
#include "view-mouse-movement.hpp"
#include "macro.hpp"

struct MouseMovementImpl {
  enum State { UpdateNewPos, UpdateOldPos, UpdateBothPos };

  QPoint oldPos;
  QPoint newPos;
  State  state;

  MouseMovementImpl () : state (UpdateNewPos) {}

  void update  (const QPoint& p) {
    if (this->state == UpdateBothPos || this->state == UpdateOldPos) {
      this->oldPos = this->newPos;
      this->state  = UpdateBothPos;
    }
    else if (this->state == UpdateNewPos) {
      this->state  = UpdateOldPos;
    }
    this->newPos = p;
  }

  void invalidate () {
    this->state = UpdateNewPos;
  }

  int dX () const {
    if (this->state == UpdateBothPos)
      return this->newPos.x () - this->oldPos.x ();
    else
      return 0;
  }

  int dY () const {
    if (this->state == UpdateBothPos)
      return this->newPos.y () - this->oldPos.y ();
    else
      return 0;
  }
};

DELEGATE_BIG4  (MouseMovement)
DELEGATE1      (void, MouseMovement, update, const QPoint&)
DELEGATE       (void, MouseMovement, invalidate)
DELEGATE_CONST (int , MouseMovement, dX)
DELEGATE_CONST (int , MouseMovement, dY)
