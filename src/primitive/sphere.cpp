#include <sstream>
#include <glm/glm.hpp>
#include "primitive/sphere.hpp"
#include "util.hpp"

struct PrimSphere::Impl {
  glm::vec3 center;
  float     radius;

  Impl (const glm::vec3& o, float r) : center (o), radius (r) {}
};

DELEGATE2_BIG6 (PrimSphere,const glm::vec3&, float)
GETTER_CONST   (const glm::vec3&,PrimSphere,center)
GETTER_CONST   (float           ,PrimSphere,radius)
SETTER         (const glm::vec3&,PrimSphere,center)
SETTER         (float           ,PrimSphere,radius)

std::ostream& operator<<(std::ostream& os, const PrimSphere& sphere) {
  os << "PrimSphere { center = " << (sphere.center ()) 
                << ", radius = " << (sphere.radius ()) << " }";
  return os;
}
