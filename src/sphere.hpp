#ifndef DILAY_SPHERE
#define DILAY_SPHERE

#include <iosfwd>
#include "fwd-glm.hpp"

class Sphere {
  public:
          Sphere            (const glm::vec3&, float);
          Sphere            (const Sphere&);
    const Sphere& operator= (const Sphere&);
         ~Sphere            ();

    const glm::vec3& origin () const;
    float            radius () const;
  private:
    class Impl;
    Impl* impl;
};

std::ostream& operator<<(std::ostream&, const Sphere&);

#endif
