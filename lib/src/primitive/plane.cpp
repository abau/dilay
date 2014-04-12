#include <glm/glm.hpp>
#include "primitive/plane.hpp"

struct PrimPlane::Impl {
  glm::vec3 point;
  glm::vec3 normal;

  Impl (const glm::vec3& p, const glm::vec3& n)
    : point  (p)
    , normal (n)
  {}
};

DELEGATE2_BIG6 (PrimPlane, const glm::vec3&, const glm::vec3&)
GETTER_CONST   (const glm::vec3&, PrimPlane, point)
GETTER_CONST   (const glm::vec3&, PrimPlane, normal)
SETTER         (const glm::vec3&, PrimPlane, point)
SETTER         (const glm::vec3&, PrimPlane, normal)
