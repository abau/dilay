#ifndef DILAY_CARVE_BRUSH
#define DILAY_CARVE_BRUSH

#include <glm/fwd.hpp>
#include "macro.hpp"

class WingedFace;
class WingedMesh;

class CarveBrush {
  public:
    DECLARE_BIG6 (CarveBrush, float, float, float, float, bool)

    float            radius          () const;
    float            detailFactor    () const;
    float            intensityFactor () const;
    float            stepWidthFactor () const;
    bool             subdivide       () const;

    void             radius          (float);
    void             detailFactor    (float);
    void             intensityFactor (float);
    void             stepWidthFactor (float);
    void             subdivide       (bool);

    float            y               (float) const;
    float            subdivThreshold ()      const;
    float            intensity       ()      const;
    float            stepWidth       ()      const;
    bool             hasPosition     ()      const;
    const glm::vec3& lastPosition    ()      const;
    const glm::vec3& position        ()      const;
    WingedMesh&      mesh            ()      const;
    WingedFace&      face            ()      const;
    bool             updatePosition  (WingedMesh&, WingedFace&, const glm::vec3&);

  private:
    IMPLEMENTATION
};

#endif
