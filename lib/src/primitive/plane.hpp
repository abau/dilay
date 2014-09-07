#ifndef DILAY_PRIMITIVE_PLANE
#define DILAY_PRIMITIVE_PLANE

#include "macro.hpp"
#include <glm/fwd.hpp>

class PrimPlane {
  public:
    DECLARE_BIG6 (PrimPlane, const glm::vec3&, const glm::vec3&)

    const glm::vec3& point  () const;
    const glm::vec3& normal () const;
          void       point  (const glm::vec3&);
          void       normal (const glm::vec3&);

  private:
    IMPLEMENTATION
};

#endif
