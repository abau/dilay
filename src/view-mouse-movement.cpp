#include "view-mouse-movement.hpp"

void MouseMovement :: update  (const QPoint& p) {
  if (this->_newPos.isDefined ()) 
    this->_oldPos.define (this->_newPos.data ());
  this->_newPos.define (p);
}

void MouseMovement :: invalidate () {
  this->_oldPos.undefine ();
  this->_newPos.undefine ();
}

int MouseMovement :: dX () const {
  if (this->_oldPos.isDefined () && this->_newPos.isDefined ())
    return this->_newPos.data ().x () - this->_oldPos.data ().x ();
  else
    return 0;
}

int MouseMovement :: dY () const {
  if (this->_oldPos.isDefined () && this->_newPos.isDefined ())
    return this->_newPos.data ().y () - this->_oldPos.data ().y ();
  else
    return 0;
}
