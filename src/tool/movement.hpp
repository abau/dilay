#ifndef DILAY_TOOL_MOVEMENT
#define DILAY_TOOL_MOVEMENT

#include <glm/fwd.hpp>
#include "macro.hpp"

class ToolMovement {
  public:
    DECLARE_BIG6 (ToolMovement)

          bool       moveXZ   (const glm::uvec2&);
          bool       moveY    (const glm::uvec2&);
    const glm::vec3& position () const;
          void       position (const glm::vec3&);

  private:
    class Impl;
    Impl* impl;
};

#endif
