#ifndef DILAY_RAY
#define DILAY_RAY

#include <iosfwd>
#include "macro.hpp"
#include "fwd-glm.hpp"

class Ray {
  public:
    DECLARE2_BIG6 (Ray,const glm::vec3&, const glm::vec3&)

    const glm::vec3& origin    () const;
    const glm::vec3& direction () const;

          glm::vec3  pointAt   (float) const;

  private:
    class Impl;
    Impl* impl;
};

std::ostream& operator<<(std::ostream&, const Ray&);

#endif
