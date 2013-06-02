#ifndef DILAY_RAY
#define DILAY_RAY

#include <iosfwd>
#include "fwd-glm.hpp"

class Ray {
  public:
          Ray            (const glm::vec3&, const glm::vec3&);
          Ray            (const Ray&);
    const Ray& operator= (const Ray&);
         ~Ray            ();

    const glm::vec3& origin    () const;
    const glm::vec3& direction () const;

          glm::vec3  pointAt   (float) const;

  private:
    class Impl;
    Impl* impl;
};

std::ostream& operator<<(std::ostream&, const Ray&);

#endif
