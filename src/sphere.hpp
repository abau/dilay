#ifndef DILAY_SPHERE
#define DILAY_SPHERE

#include <iosfwd>
#include "fwd-glm.hpp"

class SphereImpl;

class Sphere {
  public:
     Sphere            (const glm::vec3&, float);
     Sphere            (const Sphere&);
     Sphere& operator= (const Sphere&);
    ~Sphere            ();

    const glm::vec3& origin () const;
    float            radius () const;
  private:
    SphereImpl* impl;
};

std::ostream& operator<<(std::ostream&, const Sphere&);

#endif
