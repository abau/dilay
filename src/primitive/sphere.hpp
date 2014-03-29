#ifndef DILAY_PRIMITIVE_SPHERE
#define DILAY_PRIMITIVE_SPHERE

#include <iosfwd>
#include <glm/fwd.hpp>
#include "macro.hpp"

class PrimSphere {
  public:
    DECLARE_BIG6 (PrimSphere, const glm::vec3&, float)

    const glm::vec3& center () const;
    float            radius () const;

    void             center (const glm::vec3&);
    void             radius (float);
  private:
    class Impl;
    Impl* impl;
};

std::ostream& operator<<(std::ostream&, const PrimSphere&);

#endif
