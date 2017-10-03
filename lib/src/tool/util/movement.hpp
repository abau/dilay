/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL_UTIL_MOVEMENT
#define DILAY_TOOL_UTIL_MOVEMENT

#include <glm/fwd.hpp>
#include "macro.hpp"

class Camera;
class ViewPointingEvent;

class ToolUtilMovement
{
public:
  DECLARE_BIG3 (ToolUtilMovement, const Camera&, bool)
  ToolUtilMovement (const Camera&, const glm::vec3&);

  bool onPrimaryPlane () const;
  void onPrimaryPlane (bool);
  void onFreePlane (const glm::vec3&);

  glm::vec3        delta () const;
  const glm::vec3& position () const;
  bool             move (const ViewPointingEvent&);
  void             reset (const glm::vec3&);

private:
  IMPLEMENTATION
};

#endif
