#include <sstream>
#include <glm/glm.hpp>
#include "primitive/capsule.hpp"
#include "util.hpp"

struct PrimCapsule::Impl {
  const glm::vec3 point1;
  const glm::vec3 point2;
  const float     radius;

  Impl (const glm::vec3& p1, const glm::vec3& p2, float r) 
    : point1 (p1), point2 (p2), radius (r) {}
};

DELEGATE3_BIG6 (PrimCapsule, const glm::vec3&, const glm::vec3&, float)
GETTER_CONST   (const glm::vec3&,PrimCapsule,point1)
GETTER_CONST   (const glm::vec3&,PrimCapsule,point2)
GETTER_CONST   (float           ,PrimCapsule,radius)

std::ostream& operator<<(std::ostream& os, const PrimCapsule& sphere) {
  os << "PrimCapsule { point1 = " << (sphere.point1 ()) 
                 << ", point2 = " << (sphere.point2 ())
                 << ", radius = " << (sphere.radius ()) << " }";
  return os;
}
