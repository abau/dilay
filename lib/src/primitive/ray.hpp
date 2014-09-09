#ifndef DILAY_PRIMITIVE_RAY
#define DILAY_PRIMITIVE_RAY

#include <glm/fwd.hpp>
#include <iosfwd>
#include "macro.hpp"

class PrimRay {
  public:
    DECLARE_BIG4COPY (PrimRay,const glm::vec3&, const glm::vec3&)

    const glm::vec3& origin    () const;
    const glm::vec3& direction () const;

          glm::vec3  pointAt   (float) const;

  private:
    IMPLEMENTATION
};

std::ostream& operator<<(std::ostream&, const PrimRay&);

#endif
