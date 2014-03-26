#ifndef DILAY_PRIMITIVE_AABOX
#define DILAY_PRIMITIVE_AABOX

#include <glm/fwd.hpp>
#include "macro.hpp"

class PrimAABox {
  public:
    DECLARE_BIG6 (PrimAABox)

    PrimAABox (const glm::vec3&, const glm::vec3&, const glm::vec3&);
    PrimAABox (const glm::vec3&, float, float, float);
    PrimAABox (const glm::vec3&, float);

    const glm::vec3& position () const;
    const glm::vec3& maximum  () const;
    const glm::vec3& minimum  () const;
          float      xWidth   () const;
          float      yWidth   () const;
          float      zWidth   () const;

          void       position (const glm::vec3&);
          void       maximum  (const glm::vec3&);
          void       minimum  (const glm::vec3&);
          void       xWidth   (float);
          void       yWidth   (float);
          void       zWidth   (float);
          void       width    (float);

  private:
    class Impl;
    Impl* impl;
};

#endif
