#ifndef VIEW_MOUSE_MOVEMENT
#define VIEW_MOUSE_MOVEMENT

#include "fwd-glm.hpp"

class QPoint;

class MouseMovement {
  public:
          MouseMovement            ();
          MouseMovement            (const MouseMovement&);
    const MouseMovement& operator= (const MouseMovement&);
         ~MouseMovement            ();

    void              update     (const QPoint&);
    void              update     (const glm::uvec2&);
    void              invalidate ();
    glm::ivec2        delta      () const;
    const glm::uvec2& old        () const;
    const glm::uvec2& position   () const;

  private:
    class Impl;
    Impl* impl;
};

#endif
