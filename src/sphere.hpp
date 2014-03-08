#ifndef DILAY_SPHERE
#define DILAY_SPHERE

#include <iosfwd>
#include <glm/fwd.hpp>
#include "macro.hpp"

class Sphere {
  public:
    DECLARE_BIG6 (Sphere, const glm::vec3&, float)

    const glm::vec3& center () const;
    float            radius () const;
  private:
    class Impl;
    Impl* impl;
};

std::ostream& operator<<(std::ostream&, const Sphere&);

#endif
