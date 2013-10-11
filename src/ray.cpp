#include <sstream>
#include <glm/glm.hpp>
#include "ray.hpp"
#include "macro.hpp"
#include "util.hpp"

struct Ray::Impl {
  glm::vec3 origin;
  glm::vec3 direction;

  Impl (const glm::vec3& o, const glm::vec3& d) 
    : origin    (o)
    , direction (glm::normalize (d)) 
      {}

  glm::vec3 pointAt (float t) const {
    return this->origin + (this->direction * glm::vec3 (t));
  }
};

DELEGATE2_BIG4 (Ray,const glm::vec3&,const glm::vec3&)

GETTER_CONST    (const glm::vec3&, Ray, origin)
GETTER_CONST    (const glm::vec3&, Ray, direction)
DELEGATE1_CONST (glm::vec3       , Ray, pointAt, float)

std::ostream& operator<<(std::ostream& os, const Ray& ray) {
  os << "Ray { origin = "    << (ray.origin    ()) << ", " 
     <<       "direction = " << (ray.direction ()) << " }";
  return os;
}
