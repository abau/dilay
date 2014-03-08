#include <sstream>
#include <glm/glm.hpp>
#include "sphere.hpp"
#include "util.hpp"

struct Sphere::Impl {
  glm::vec3 center;
  float     radius;

  Impl (const glm::vec3& o, float r) : center (o), radius (r) {}

  bool intersects (const glm::vec3& v) const {
    return glm::distance (v, this->center) <= this->radius;
  }
};

DELEGATE2_BIG6  (Sphere, const glm::vec3&, float)
GETTER_CONST    (const glm::vec3&, Sphere, center)
GETTER_CONST    (float           , Sphere, radius)
DELEGATE1_CONST (bool            , Sphere, intersects, const glm::vec3&)

std::ostream& operator<<(std::ostream& os, const Sphere& sphere) {
  os << "Sphere { center = " << (sphere.center ()) 
            << ", radius = " << (sphere.radius ()) << " }";
  return os;
}
