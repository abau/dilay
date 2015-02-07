#include <sstream>
#include "primitive/plane.hpp"
#include "util.hpp"

PrimPlane :: PrimPlane (const glm::vec3& p, const glm::vec3& n)
  : _point  (p)
  , _normal (glm::normalize (n))
{}

float PrimPlane :: distance (const glm::vec3& p) const {
  return glm::dot (this->_normal, p - this->_point);
}

std::ostream& operator<<(std::ostream& os, const PrimPlane& plane) {
  os << "PrimPlane { point = "  << (plane.point  ())
               << ", normal = " << (plane.normal ()) << " }";
  return os;
}
