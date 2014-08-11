#include <sstream>
#include <glm/glm.hpp>
#include "primitive/sphere.hpp"
#include "util.hpp"

struct PrimSphere::Impl {
  const glm::vec3 center;
  const float     radius;

  Impl (const glm::vec3& o, float r) : center (o), radius (r) {}

  Impl (const PrimSphere& s, const glm::mat4x4& m) 
    : Impl (Util::transformPosition (m, s.center ()), s.radius ()) {}
};

DELEGATE2_BIG6        (PrimSphere, const glm::vec3&, float)
DELEGATE2_CONSTRUCTOR (PrimSphere, const PrimSphere&, const glm::mat4x4&)
GETTER_CONST   (const glm::vec3&,PrimSphere,center)
GETTER_CONST   (float           ,PrimSphere,radius)

std::ostream& operator<<(std::ostream& os, const PrimSphere& sphere) {
  os << "PrimSphere { center = " << (sphere.center ()) 
                << ", radius = " << (sphere.radius ()) << " }";
  return os;
}
