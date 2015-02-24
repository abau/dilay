#ifndef DILAY_SCULPT_BRUSH_SMOOTH
#define DILAY_SCULPT_BRUSH_SMOOTH

#include "sculpt-brush.hpp"

class SculptBrushNothing : public SculptBrush {
  public:
    DECLARE_BIG6 (SculptBrushNothing)

  private:
    IMPLEMENTATION

    void runSculpt (AffectedFaces&, ActionUnitOn <WingedMesh>&) const;
};

#endif
