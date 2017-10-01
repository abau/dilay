/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL_UTIL_SCALING
#define DILAY_TOOL_UTIL_SCALING

#include <glm/fwd.hpp>
#include "macro.hpp"

class Camera;
class ViewPointingEvent;

class ToolUtilScaling
{
public:
  DECLARE_BIG3 (ToolUtilScaling, const Camera&)

  float factor () const;
  bool  move (const ViewPointingEvent&);
  void  reset (const glm::vec3&, const glm::vec3&);

private:
  IMPLEMENTATION
};

#endif
