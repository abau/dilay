#ifndef DILAY_TOOL_MOVEMENT
#define DILAY_TOOL_MOVEMENT

#include <glm/fwd.hpp>
#include "macro.hpp"

class QMouseEvent;

enum class Movement { X, Y, Z, XY, XZ, YZ, CameraPlane, PrimaryPlane };

class ToolMovement {
  public:
    DECLARE_BIG6 (ToolMovement, Movement, const glm::ivec2&)

    const glm::vec3& originalPosition () const;
          glm::vec3  delta            () const;
    const glm::vec3& position         () const;
          void       position         (const glm::vec3&);
          bool       byMouseEvent     (QMouseEvent&);
          bool       onCameraPlane    (const glm::ivec2&, glm::vec3&) const;

  private:
    IMPLEMENTATION
};

#endif
