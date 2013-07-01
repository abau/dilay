#include <glm/glm.hpp>
#include <QPoint>
#include "view-mouse-movement.hpp"
#include "macro.hpp"

struct MouseMovement::Impl {
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
};

DELEGATE_BIG4  (MouseMovement)
DELEGATE1      (void             , MouseMovement, update, const QPoint&)
DELEGATE       (void             , MouseMovement, invalidate)
DELEGATE_CONST (glm::ivec2       , MouseMovement, delta)
GETTER         (const glm::uvec2&, MouseMovement, old)
GETTER         (const glm::uvec2&, MouseMovement, position)
