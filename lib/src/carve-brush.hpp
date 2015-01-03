#ifndef DILAY_CARVE_BRUSH
#define DILAY_CARVE_BRUSH

#include <glm/fwd.hpp>
#include "macro.hpp"

class WingedMesh;

class CarveBrush {
  public:
    DECLARE_BIG6 (CarveBrush, float, float, float, float)

    float            y               (float) const;
    float            radius          ()      const;
    float            detailFactor    ()      const;
    float            subdivThreshold ()      const;
    float            intensityFactor ()      const;
    float            intensity       ()      const;
    float            stepWidthFactor ()      const;
    float            stepWidth       ()      const;
    bool             hasPosition     ()      const;
    const glm::vec3& lastPosition    ()      const;
    const glm::vec3& position        ()      const;
    WingedMesh&      mesh            ()      const;

    bool             updatePosition  (WingedMesh&, const glm::vec3&);
    void             radius          (float);
    void             detailFactor    (float);
    void             intensityFactor (float);
    void             stepWidthFactor (float);

  private:
    IMPLEMENTATION
};

#endif
