/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL_SCALING
#define DILAY_TOOL_SCALING

#include <glm/fwd.hpp>
#include "macro.hpp"

class Camera;
class QMouseEvent;

class ToolUtilScaling {
  public:
    DECLARE_BIG3 (ToolUtilScaling, const Camera&)

    float factor        () const;
    bool  move          (const QMouseEvent&);
    void  resetPosition (const glm::vec3&, const glm::vec3&);

  private:
    IMPLEMENTATION
};

#endif
