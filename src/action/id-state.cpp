#include "action/id-state.hpp"
#include "macro.hpp"
#include "id.hpp"

struct ActionIdState :: Impl {
  IdPrimitive state;

  Impl () : state (IdObject::invalidId ().get ())
    {}

  void run () { 
    this->state = IdObject::_currentId;
  }

  void toggle () {
    IdPrimitive currentId = IdObject::_currentId;
    IdObject::_currentId  = this->state;
    this->state           = currentId;
  }

  void undo () { this->toggle (); }
  void redo () { this->toggle (); }
};

DELEGATE_CONSTRUCTOR (ActionIdState)
DELEGATE_DESTRUCTOR  (ActionIdState)
DELEGATE             (void, ActionIdState, run)
DELEGATE             (void, ActionIdState, undo)
DELEGATE             (void, ActionIdState, redo)
