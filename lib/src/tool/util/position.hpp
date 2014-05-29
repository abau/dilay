#ifndef DILAY_TOOL_UTIL_POSITION
#define DILAY_TOOL_UTIL_POSITION

#include <glm/fwd.hpp>
#include "tool/util.hpp"

class QMouseEvent;
class ToolMovement;

class ToolUtilPosition : public ToolUtil {
  public:
    DECLARE_BIG3 (ToolUtilPosition, Tool&)

    const ToolMovement& movement          () const;
    const glm::vec3&    position          () const;
    void                position          (const glm::vec3& pos);
    bool                runMouseMoveEvent (QMouseEvent& e);

  private:
    class Impl;
    Impl* impl;
};

#endif
