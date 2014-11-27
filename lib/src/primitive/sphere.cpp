#include <sstream>
#include "primitive/sphere.hpp"
#include "util.hpp"

PrimSphere :: PrimSphere (const glm::vec3& o, float r) 
  : _center (o)
  , _radius (r) 
{}

std::ostream& operator<<(std::ostream& os, const PrimSphere& sphere) {
  os << "PrimSphere { center = " << (sphere.center ()) 
                << ", radius = " << (sphere.radius ()) << " }";
  return os;
}
