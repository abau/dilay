#ifndef DILAY_SCULPT_BRUSH_CARVE
#define DILAY_SCULPT_BRUSH_CARVE

#include "sculpt-brush.hpp"

class SculptBrushCarve : public SculptBrush {
  public:
    DECLARE_BIG6 (SculptBrushCarve)

    float intensityFactor () const;
    void  intensityFactor (float);

  private:
    IMPLEMENTATION

    void runSculpt (AffectedFaces&, ActionUnitOn <WingedMesh>&) const;
};

#endif
