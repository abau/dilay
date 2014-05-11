#ifndef DILAY_TOOL_MOVEMENT
#define DILAY_TOOL_MOVEMENT

#include <glm/fwd.hpp>
#include "macro.hpp"

class QMouseEvent;
class Tool;

class ToolMovement {
  public:
    DECLARE_BIG6 (ToolMovement, Tool*)

    const glm::vec3& position      () const;
          void       position      (const glm::vec3&);
          bool       byMouseEvent  (QMouseEvent*);
          bool       byScreenPos   ();
          bool       onCameraPlane (const glm::ivec2&, glm::vec3*) const;

  private:
    class Impl;
    Impl* impl;
};

#endif
