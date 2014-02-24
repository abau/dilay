#include <glm/glm.hpp>
#include "plane.hpp"

struct Plane::Impl {
  glm::vec3 point;
  glm::vec3 normal;

  Impl (const glm::vec3& p, const glm::vec3& n)
    : point  (p)
    , normal (n)
  {}
};

DELEGATE2_BIG6 (Plane, const glm::vec3&, const glm::vec3&)
GETTER_CONST   (const glm::vec3&, Plane, point)
GETTER_CONST   (const glm::vec3&, Plane, normal)
SETTER         (const glm::vec3&, Plane, point)
SETTER         (const glm::vec3&, Plane, normal)
