#ifndef DILAY_RAY
#define DILAY_RAY

#include <iosfwd>
#include "fwd-glm.hpp"

class RayImpl;

class Ray {
  public:
     Ray            (const glm::vec3&, const glm::vec3&);
     Ray            (const Ray&);
     Ray& operator= (const Ray&);
    ~Ray            ();

    const glm::vec3& origin    () const;
    const glm::vec3& direction () const;

    glm::vec3 pointAt (float) const;

  private:
    RayImpl* impl;
};

std::ostream& operator<<(std::ostream&, const Ray&);

#endif
