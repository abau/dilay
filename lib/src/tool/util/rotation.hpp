/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL_UTIL_ROTATION
#define DILAY_TOOL_UTIL_ROTATION

#include <functional>
#include <glm/fwd.hpp>
#include "macro.hpp"

class Camera;
class ViewPointingEvent;

class ToolUtilRotation
{
public:
  DECLARE_BIG3 (ToolUtilRotation, const Camera&)

  const glm::vec3& axis () const;
  float            angle () const;
  glm::mat4x4      matrix () const;
  bool             rotate (const ViewPointingEvent&);
  void             reset (const glm::vec3&);
  void             reset (const glm::vec3&, const glm::vec3&);

private:
  IMPLEMENTATION
};

#endif
