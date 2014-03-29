#ifndef DILAY_TOOL_MOVEMENT
#define DILAY_TOOL_MOVEMENT

#include <glm/fwd.hpp>
#include "macro.hpp"

class QMouseEvent;
class ViewPropertiesMovement;

class ToolMovement {
  public:
    DECLARE_BIG6 (ToolMovement)

    const glm::vec3& position      () const;
          void       position      (const glm::vec3&);
          bool       byMouseEvent  (ViewPropertiesMovement*, QMouseEvent*);
          bool       byScreenPos   (ViewPropertiesMovement*, const glm::ivec2&);
          bool       onCameraPlane (const glm::ivec2&, glm::vec3*) const;

  private:
    class Impl;
    Impl* impl;
};

#endif
