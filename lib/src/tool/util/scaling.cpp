/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "camera.hpp"
#include "intersection.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "tool/util/scaling.hpp"
#include "util.hpp"
#include "view/pointing-event.hpp"

struct ToolUtilScaling::Impl
{
  const Camera&              camera;
  std::unique_ptr<PrimPlane> plane;
  glm::vec3                  previousPosition;
  glm::vec3                  position;

  Impl (const Camera& cam)
    : camera (cam)
  {
  }

  float factor () const
  {
    assert (this->plane);

    const float d = glm::distance (this->position, this->plane->point ());
    const float pd = glm::distance (this->previousPosition, this->plane->point ());

    return pd <= Util::epsilon () ? 1.0f : d / pd;
  }

  bool intersects (const glm::ivec2& p, glm::vec3& i) const
  {
    assert (this->plane);

    const PrimRay ray = this->camera.ray (p);
    float         t;

    if (IntersectionUtil::intersects (ray, *this->plane, &t))
    {
      i = ray.pointAt (t);
      return true;
    }
    return false;
  }

  bool move (const ViewPointingEvent& e)
  {
    glm::vec3 p;
    if (this->plane && this->intersects (e.ivec2 (), p))
    {
      this->previousPosition = this->position;
      this->position = p;
      return true;
    }
    else
    {
      return false;
    }
  }

  void resetPosition (const glm::vec3& origin, const glm::vec3& reference)
  {
    this->plane = std::make_unique<PrimPlane> (origin, this->camera.toEyePoint ());

    const PrimRay ray (this->camera.position (),
                       glm::normalize (reference - this->camera.position ()));
    float t;

    if (IntersectionUtil::intersects (ray, *this->plane, &t))
    {
      this->previousPosition = origin;
      this->position = origin;
    }
    else
    {
      this->plane.reset ();
    }
  }
};

DELEGATE1_BIG3 (ToolUtilScaling, const Camera&)
DELEGATE_CONST (float, ToolUtilScaling, factor)
DELEGATE1 (bool, ToolUtilScaling, move, const ViewPointingEvent&)
DELEGATE2 (void, ToolUtilScaling, resetPosition, const glm::vec3&, const glm::vec3&)
