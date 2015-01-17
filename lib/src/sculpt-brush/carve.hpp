#ifndef DILAY_SCULPT_BRUSH_CARVE
#define DILAY_SCULPT_BRUSH_CARVE

#include "sculpt-brush.hpp"

class SculptBrushCarve : public SculptBrush {
  public:
    DECLARE_BIG6 (SculptBrushCarve)

    float            intensityFactor () const;
    float            stepWidthFactor () const;
    const glm::vec3& lastPosition    () const;

    float            intensity       () const;
    float            stepWidth       () const;

    void             intensityFactor (float);
    void             stepWidthFactor (float);

  private:
    IMPLEMENTATION

    void runSculpt (AffectedFaces&, ActionUnitOn <WingedMesh>&) const;
    bool runUpdate (WingedMesh&, WingedFace&, const glm::vec3&);
};

#endif
