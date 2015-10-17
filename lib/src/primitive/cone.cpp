/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/gtc/epsilon.hpp>
#include <sstream>
#include "primitive/cone.hpp"
#include "util.hpp"

PrimCone :: PrimCone (const glm::vec3& c1, float r1, const glm::vec3& c2, float r2, float l)
  : _center1     (r1 > r2 ? c1 : c2)
  , _radius1     (r1 > r2 ? r1 : r2) 
  , _center2     (r1 > r2 ? c2 : c1)
  , _radius2     (r1 > r2 ? r2 : r1) 
  , _length      (l)
  , _direction   ((this->_center2 - this->_center1) / glm::vec3 (l))
  , _isCylinder  (glm::epsilonEqual (r1, r2, Util::epsilon ()))
  , _apex        (this->_isCylinder ? glm::vec3 (0.0f)
                                    : this->_center1 + ( this->_radius1
                                                       * (this->_center2 - this->_center1)
                                                       / (this->_radius1 - this->_radius2) ))
  , _alpha       (glm::atan ((this->_radius1 - this->_radius2) / l))
  , _sinSqrAlpha (glm::sin (this->_alpha) * glm::sin (this->_alpha))
  , _cosSqrAlpha (glm::cos (this->_alpha) * glm::cos (this->_alpha))
{}

PrimCone :: PrimCone (const glm::vec3& c1, float r1, const glm::vec3& c2, float r2) 
  : PrimCone (c1, r1, c2, r2, glm::distance (c1, c2))
{}

glm::vec3 PrimCone :: projPointAt (float t) const {
  return this->_center1 + (t * this->_direction);
}

glm::vec3 PrimCone :: normalAt (const glm::vec3& pointAt, float tCone) const {
  const glm::vec3 projP = this->projPointAt (tCone);
  const glm::vec3 diff  = glm::normalize (pointAt - projP);
  const glm::vec3 slope = (this->_center2 + (this->_radius2 * diff))
                        - (this->_center1 + (this->_radius1 * diff));
  const glm::vec3 tang  = glm::cross (diff, this->_direction);

  return glm::normalize (glm::cross (slope, tang));
}

std::ostream& operator<<(std::ostream& os, const PrimCone& cone) {
  os << "PrimCone { center1 = " << (cone.center1 ()) 
              << ", radius1 = " << (cone.radius1 ())
              << ", center2 = " << (cone.center2 ()) 
              << ", radius2 = " << (cone.radius2 ()) << " }";
  return os;
}
