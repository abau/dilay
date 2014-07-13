#ifndef DILAY_PRIMITIVE_RAY
#define DILAY_PRIMITIVE_RAY

#include <iosfwd>
#include <glm/fwd.hpp>
#include "macro.hpp"

class PrimRay {
  public:
    DECLARE_BIG6 (PrimRay,const glm::vec3&, const glm::vec3&)

    PrimRay (const PrimRay&, const glm::mat4x4&);

    const glm::vec3& origin    () const;
    const glm::vec3& direction () const;

          glm::vec3  pointAt   (float) const;

  private:
    class Impl;
    Impl* impl;
};

std::ostream& operator<<(std::ostream&, const PrimRay&);

#endif
