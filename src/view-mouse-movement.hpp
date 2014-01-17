#ifndef VIEW_MOUSE_MOVEMENT
#define VIEW_MOUSE_MOVEMENT

#include <glm/fwd.hpp>
#include "macro.hpp"

class QPoint;

class MouseMovement {
  public:
    DECLARE_BIG6 (MouseMovement)

    void              update       (const QPoint&);
    void              update       (const glm::uvec2&);
    void              invalidate   ();
    glm::ivec2        delta        () const;
    const glm::uvec2& old          () const;
    const glm::uvec2& position     () const;
    bool              hasOld       () const;
    bool              hasPosition  () const;

  private:
    class Impl;
    Impl* impl;
};

#endif
