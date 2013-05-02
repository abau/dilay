#include <sstream>
#include <glm/glm.hpp>
#include "sphere.hpp"
#include "macro.hpp"
#include "util.hpp"

struct SphereImpl {
  glm::vec3 origin;
  float     radius;

  SphereImpl (const glm::vec3& o, float r) : origin (o), radius (r) {}
};

DELEGATE2_BIG4 (Sphere,const glm::vec3&, float)
GETTER         (const glm::vec3&,Sphere,origin)
GETTER         (float           ,Sphere,radius)

std::ostream& operator<<(std::ostream& os, const Sphere& sphere) {
  os << "Sphere { origin = " << (sphere.origin ()) 
            << ", radius = " << (sphere.radius ()) << " }";
  return os;
}
