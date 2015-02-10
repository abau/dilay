#ifndef DILAY_TOOL_MOVEMENT
#define DILAY_TOOL_MOVEMENT

#include <glm/fwd.hpp>
#include "macro.hpp"

class Camera;
class QMouseEvent;

enum class MovementConstraint { XAxis, YAxis, ZAxis
                              , XYPlane, XZPlane, YZPlane
                              , CameraPlane, VerticalCameraPlane
                              , PrimaryPlane, Explicit
                              };
class ToolUtilMovement {
  public:
    DECLARE_BIG4COPY (ToolUtilMovement, const Camera&, const glm::vec3&, MovementConstraint)

    MovementConstraint constraint       () const;
          void         constraint       (MovementConstraint);
          void         explicitPlane    (const glm::vec3&);
    const glm::vec3&   originalPosition () const;
          glm::vec3    delta            () const;
          void         delta            (const glm::vec3&);
    const glm::vec3&   position         () const;
          void         position         (const glm::vec3&);
          bool         move             (const glm::ivec2&, bool = false);
          bool         move             (QMouseEvent&);
          void         resetPosition    (const glm::vec3&);

  private:
    IMPLEMENTATION
};

#endif
