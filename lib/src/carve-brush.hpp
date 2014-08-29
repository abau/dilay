#ifndef DILAY_CARVE_BRUSH
#define DILAY_CARVE_BRUSH

#include <glm/fwd.hpp>
#include "macro.hpp"

class WingedMesh;

class CarveBrush {
  public:
    DECLARE_BIG6 (CarveBrush, float, float, float, float)

    float            y            (float) const;
    float            width        ()      const;
    float            height       ()      const;
    float            detail       ()      const;
    float            stepWidth    ()      const;
    bool             hasPosition  ()      const;
    const glm::vec3& lastPosition ()      const;
    const glm::vec3& position     ()      const;
    WingedMesh&      mesh         ()      const;

    bool             updatePosition (WingedMesh&, const glm::vec3&);
    void             width          (float);
    void             height         (float);
    void             detail         (float);
    void             stepWidth      (float);

  private:
    class Impl;
    Impl* impl;
};

#endif
