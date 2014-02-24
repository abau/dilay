#ifndef DILAY_PLANE
#define DILAY_PLANE

#include "macro.hpp"
#include <glm/fwd.hpp>

class Plane {
  public:
    DECLARE_BIG6 (Plane, const glm::vec3&, const glm::vec3&)

    const glm::vec3& point  () const;
    const glm::vec3& normal () const;
          void       point  (const glm::vec3&);
          void       normal (const glm::vec3&);

  private:
    class Impl;
    Impl* impl;
};

#endif
