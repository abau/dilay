#ifndef DILAY_TOOL_MOVEMENT
#define DILAY_TOOL_MOVEMENT

#include <glm/fwd.hpp>
#include "macro.hpp"

class QMouseEvent;
class QPoint;

class ToolMovement {
  public:
    DECLARE_BIG6 (ToolMovement)

          bool       moveXZ       (const glm::uvec2&);
          bool       moveXZ       (const QPoint&);
          bool       moveY        (const glm::uvec2&);
          bool       moveY        (const QPoint&);
    const glm::vec3& position     () const;
          void       position     (const glm::vec3&);
          bool       byMouseEvent (QMouseEvent*);

  private:
    class Impl;
    Impl* impl;
};

#endif
