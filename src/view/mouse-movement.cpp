#include <glm/glm.hpp>
#include <QPoint>
#include "view/mouse-movement.hpp"

struct ViewMouseMovement::Impl {
  enum State { UpdateNewPos, UpdateOldPos, UpdateBothPos };

  glm::uvec2 old;
  glm::uvec2 position;
  State      state;

  Impl () : state (UpdateNewPos) {}

  void update (const QPoint& p) {
    return this->update (glm::uvec2 (p.x (), p.y ()));
  }

  void update (const glm::uvec2& p) {
    if (this->state == UpdateBothPos || this->state == UpdateOldPos) {
      this->old   = this->position;
      this->state = UpdateBothPos;
    }
    else if (this->state == UpdateNewPos) {
      this->state  = UpdateOldPos;
    }
    this->position = p;
  }

  void invalidate () {
    this->state = UpdateNewPos;
  }

  glm::ivec2 delta () const {
    if (this->state == UpdateBothPos) {
      return glm::ivec2 ( this->position.x - this->old.x
                        , this->position.y - this->old.y);
    }
    else
      return glm::ivec2 (0);
  }

  bool hasPosition () const { return this->state != UpdateNewPos; }
  bool hasOld      () const { return this->state == UpdateBothPos; }
};

DELEGATE_BIG6  (ViewMouseMovement)
DELEGATE1      (void             , ViewMouseMovement, update, const QPoint&)
DELEGATE       (void             , ViewMouseMovement, invalidate)
DELEGATE_CONST (glm::ivec2       , ViewMouseMovement, delta)
DELEGATE_CONST (bool             , ViewMouseMovement, hasPosition)
DELEGATE_CONST (bool             , ViewMouseMovement, hasOld)
GETTER_CONST   (const glm::uvec2&, ViewMouseMovement, old)
GETTER_CONST   (const glm::uvec2&, ViewMouseMovement, position)
