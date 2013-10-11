#include <sstream>
#include <glm/glm.hpp>
#include "sphere.hpp"
#include "macro.hpp"
#include "util.hpp"

struct Sphere::Impl {
  glm::vec3 center;
  float     radius;

  Impl (const glm::vec3& o, float r) : center (o), radius (r) {}
};

DELEGATE2_BIG4 (Sphere,const glm::vec3&, float)
GETTER_CONST   (const glm::vec3&,Sphere,center)
GETTER_CONST   (float           ,Sphere,radius)

std::ostream& operator<<(std::ostream& os, const Sphere& sphere) {
  os << "Sphere { center = " << (sphere.center ()) 
            << ", radius = " << (sphere.radius ()) << " }";
  return os;
}
