#include <sstream>
#include "primitive/ray.hpp"
#include "util.hpp"

PrimRay :: PrimRay (bool l, const glm::vec3& o, const glm::vec3& d) 
  : _isLine    (l)
  , _origin    (o)
  , _direction (glm::normalize (d)) 
{}

PrimRay :: PrimRay (const glm::vec3& o, const glm::vec3& d) 
  : PrimRay (false, o, d)
{}

glm::vec3 PrimRay :: pointAt (float t) const {
  assert (t >= 0.0f || this->_isLine);
  return this->_origin + (this->_direction * glm::vec3 (t));
}

std::ostream& operator<<(std::ostream& os, const PrimRay& ray) {
  os << "PrimRay { origin = "    << (ray.origin    ())
             << ", direction = " << (ray.direction ()) << " }";
  return os;
}
