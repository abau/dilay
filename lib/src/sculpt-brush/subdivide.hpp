#ifndef DILAY_SCULPT_BRUSH_SUBDIVIDE
#define DILAY_SCULPT_BRUSH_SUBDIVIDE

#include "sculpt-brush.hpp"

class SculptBrushSubdivide : public SculptBrush {
  public:
    DECLARE_BIG6 (SculptBrushSubdivide)

  private:
    IMPLEMENTATION

    void runSculpt (AffectedFaces&, ActionUnitOn <WingedMesh>&) const;
};

#endif
