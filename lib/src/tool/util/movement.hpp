/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL_UTIL_MOVEMENT
#define DILAY_TOOL_UTIL_MOVEMENT

#include <functional>
#include <glm/fwd.hpp>
#include "macro.hpp"

class Camera;
class ViewPointingEvent;
class ViewTwoColumnGrid;

class ToolUtilMovement
{
public:
  enum class FixedConstraint
  {
    XAxis,
    YAxis,
    ZAxis,
    XYPlane,
    XZPlane,
    YZPlane,
    CameraPlane,
    PrimaryPlane
  };

  // Constrained to fixed plane or axis
  DECLARE_BIG3 (ToolUtilMovement, const Camera&, FixedConstraint)

  FixedConstraint fixedConstraint () const;
  void            fixedConstraint (FixedConstraint);
  void            addFixedProperties (ViewTwoColumnGrid&, const std::function<void()>&);

  // Constrained to free plane
  ToolUtilMovement (const Camera&, const glm::vec3&);

  const glm::vec3& freePlaneConstraint () const;
  void             freePlaneConstraint (const glm::vec3&);

  glm::vec3        delta () const;
  const glm::vec3& position () const;
  bool             move (const ViewPointingEvent&, bool);
  void             resetPosition (const glm::vec3&);

private:
  IMPLEMENTATION
};

#endif
