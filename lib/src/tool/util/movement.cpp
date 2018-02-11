/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "camera.hpp"
#include "dimension.hpp"
#include "intersection.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "tool/util/movement.hpp"
#include "view/pointing-event.hpp"

namespace
{
  enum class MovementPlane
  {
    Camera,
    Primary,
    Free
  };
}

struct ToolUtilMovement::Impl
{
  const Camera& camera;
  MovementPlane plane;
  glm::vec3     planeNormal;
  glm::vec3     previousPosition;
  glm::vec3     position;

  Impl (const Camera& cam, bool p)
    : camera (cam)
    , previousPosition (glm::vec3 (0.0f))
    , position (glm::vec3 (0.0f))
  {
    this->onPrimaryPlane (p);
  }

  Impl (const Camera& cam, const glm::vec3& n)
    : camera (cam)
    , previousPosition (glm::vec3 (0.0f))
    , position (glm::vec3 (0.0f))
  {
    this->onFreePlane (n);
  }

  void setPlaneNormal ()
  {
    if (this->plane == MovementPlane::Camera)
    {
      this->planeNormal = this->camera.toEyePoint ();
    }
    else if (this->plane == MovementPlane::Primary)
    {
      this->planeNormal = DimensionUtil::vector (this->camera.primaryDimension ());
    }
  }

  bool onPrimaryPlane () const { return this->plane == MovementPlane::Primary; }

  void onPrimaryPlane (bool yes)
  {
    this->plane = yes ? MovementPlane::Primary : MovementPlane::Camera;
    this->setPlaneNormal ();
  }

  void onFreePlane (const glm::vec3& n)
  {
    this->plane = MovementPlane::Free;
    this->planeNormal = n;
  }

  glm::vec3 delta () const { return this->position - this->previousPosition; }

  bool intersects (const glm::ivec2& p, glm::vec3& i)
  {
    this->setPlaneNormal ();

    const PrimRay   ray = this->camera.ray (p);
    const PrimPlane plane (this->position, this->planeNormal);
    float           t;

    if (IntersectionUtil::intersects (ray, plane, &t))
    {
      i = ray.pointAt (t);
      return true;
    }
    return false;
  }

  bool move (const ViewPointingEvent& e)
  {
    glm::vec3 i;
    if (this->intersects (e.position (), i))
    {
      this->previousPosition = this->position;
      this->position = i;
      return true;
    }
    else
    {
      return false;
    }
  }

  void reset (const glm::vec3& p)
  {
    this->previousPosition = p;
    this->position = p;
  }
};

DELEGATE2_BIG3 (ToolUtilMovement, const Camera&, bool)
DELEGATE2_CONSTRUCTOR (ToolUtilMovement, const Camera&, const glm::vec3&)
DELEGATE_CONST (bool, ToolUtilMovement, onPrimaryPlane)
DELEGATE1 (void, ToolUtilMovement, onPrimaryPlane, bool)
DELEGATE1 (void, ToolUtilMovement, onFreePlane, const glm::vec3&)
DELEGATE_CONST (glm::vec3, ToolUtilMovement, delta)
GETTER_CONST (const glm::vec3&, ToolUtilMovement, position)
DELEGATE1 (bool, ToolUtilMovement, move, const ViewPointingEvent&)
DELEGATE1 (void, ToolUtilMovement, reset, const glm::vec3&)
