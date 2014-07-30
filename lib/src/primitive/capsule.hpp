#ifndef DILAY_PRIMITIVE_CAPSULE
#define DILAY_PRIMITIVE_CAPSULE

#include <iosfwd>
#include <glm/fwd.hpp>
#include "macro.hpp"

class PrimCapsule {
  public:
    DECLARE_BIG6 (PrimCapsule, const glm::vec3&, const glm::vec3&, float);

    const glm::vec3& point1 () const;
    const glm::vec3& point2 () const;
    float            radius () const;

  private:
    class Impl;
    Impl* impl;
};

std::ostream& operator<<(std::ostream&, const PrimCapsule&);

#endif
