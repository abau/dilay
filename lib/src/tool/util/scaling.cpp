/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QMouseEvent>
#include "camera.hpp"
#include "intersection.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "tool/util/scaling.hpp"
#include "view/util.hpp"

struct ToolUtilScaling::Impl {
  const Camera&               camera;
  std::unique_ptr <PrimPlane> plane;
  glm::vec3                   position;
  float                       originalDistance;
  float                       projectedDistance;

  Impl (const Camera& cam) 
    : camera (cam)
  {}

  float delta () const {
    assert (this->plane);
    return this->originalDistance * glm::distance (this->position, this->plane->point ()) 
                                  / this->projectedDistance;
  }

  bool intersects (const glm::ivec2& p, glm::vec3& i) const {
    assert (this->plane);

    const PrimRay ray = this->camera.ray (p);
          float   t;
    
    if (IntersectionUtil::intersects (ray, *this->plane, &t)) {
      i = ray.pointAt (t);
      return true;
    }
    return false;
  }

  bool move (const QMouseEvent& e) {
    return this->plane ? this->intersects (ViewUtil::toIVec2 (e), this->position)
                       : false;
  }

  void resetPosition (const glm::vec3& origin, const glm::vec3& reference) {
    this->plane = std::make_unique <PrimPlane> (origin, this->camera.toEyePoint ());

    const PrimRay ray ( this->camera.position ()
                      , glm::normalize (reference - this->camera.position ()) );
    float t ;

    if (IntersectionUtil::intersects (ray, *this->plane, &t)) {
      this->position          = origin;
      this->originalDistance  = glm::distance (origin, reference);
      this->projectedDistance = glm::distance (origin, ray.pointAt (t));
    }
    else {
      this->plane.reset ();
    }
  }
};

DELEGATE1_BIG3 (ToolUtilScaling, const Camera&)
DELEGATE_CONST (float, ToolUtilScaling, delta)
DELEGATE1      (bool , ToolUtilScaling, move, const QMouseEvent&)
DELEGATE2      (void , ToolUtilScaling, resetPosition, const glm::vec3&, const glm::vec3&)
