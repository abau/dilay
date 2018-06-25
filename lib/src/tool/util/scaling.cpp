/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
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

  float factor (float previousDistance, float distance) const
  {
    if (Util::almostEqual (previousDistance, 0.0f))
    {
      return 1.0f;
    }
    else
    {
      return glm::clamp (distance / previousDistance, 0.75f, 1.25f);
    }
  }

  float factor () const
  {
    assert (this->plane);

    const float pd = glm::distance (this->previousPosition, this->plane->point ());
    const float d = glm::distance (this->position, this->plane->point ());

    return this->factor (pd, d);
  }

  float factorRight () const
  {
    const glm::vec3  prevDelta = this->previousPosition - this->plane->point ();
    const glm::vec3  delta = this->position - this->plane->point ();
    const glm::vec3& right = this->camera.right ();

    return this->factor (glm::dot (right, prevDelta), glm::dot (right, delta));
  }

  float factorUp () const
  {
    const glm::vec3  prevDelta = this->previousPosition - this->plane->point ();
    const glm::vec3  delta = this->position - this->plane->point ();
    const glm::vec3& up = this->camera.realUp ();

    return this->factor (glm::dot (up, prevDelta), glm::dot (up, delta));
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
    if (this->plane && this->intersects (e.position (), p))
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

  void reset (const glm::vec3& origin, const glm::vec3& reference)
  {
    this->plane = std::make_unique<PrimPlane> (origin, this->camera.toEyePoint ());

    const PrimRay ray (this->camera.position (),
                       glm::normalize (reference - this->camera.position ()));
    float         t;

    if (IntersectionUtil::intersects (ray, *this->plane, &t))
    {
      this->previousPosition = ray.pointAt (t);
      this->position = ray.pointAt (t);
    }
    else
    {
      this->plane.reset ();
    }
  }
};

DELEGATE1_BIG3 (ToolUtilScaling, const Camera&)
DELEGATE_CONST (float, ToolUtilScaling, factor)
DELEGATE_CONST (float, ToolUtilScaling, factorRight)
DELEGATE_CONST (float, ToolUtilScaling, factorUp)
DELEGATE1 (bool, ToolUtilScaling, move, const ViewPointingEvent&)
DELEGATE2 (void, ToolUtilScaling, reset, const glm::vec3&, const glm::vec3&)
