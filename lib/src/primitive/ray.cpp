#include <glm/glm.hpp>
#include <sstream>
#include "primitive/ray.hpp"
#include "util.hpp"

struct PrimRay::Impl {
  const bool      isLine;
  const glm::vec3 origin;
  const glm::vec3 direction;

  Impl (bool l, const glm::vec3& o, const glm::vec3& d) 
    : isLine    (l)
    , origin    (o)
    , direction (glm::normalize (d)) 
  {}

  Impl (const glm::vec3& o, const glm::vec3& d) : Impl (false, o, d) {}

  glm::vec3 pointAt (float t) const {
    assert (t >= 0.0f || this->isLine);
    return this->origin + (this->direction * glm::vec3 (t));
  }
};

DELEGATE3_BIG4COPY    (PrimRay, bool, const glm::vec3&, const glm::vec3&)
DELEGATE2_CONSTRUCTOR (PrimRay, const glm::vec3&, const glm::vec3&)
GETTER_CONST    (bool            , PrimRay, isLine)
GETTER_CONST    (const glm::vec3&, PrimRay, origin)
GETTER_CONST    (const glm::vec3&, PrimRay, direction)
DELEGATE1_CONST (glm::vec3       , PrimRay, pointAt, float)

std::ostream& operator<<(std::ostream& os, const PrimRay& ray) {
  os << "PrimRay { origin = "    << (ray.origin    ())
             << ", direction = " << (ray.direction ()) << " }";
  return os;
}
