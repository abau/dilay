#ifndef DILAY_SCULPT_BRUSH_CARVE
#define DILAY_SCULPT_BRUSH_CARVE

#include "sculpt-brush.hpp"

class SculptBrushCarve : public SculptBrush {
  public:
    DECLARE_BIG6 (SculptBrushCarve)

    float        intensityFactor     () const;
    unsigned int order               () const;
    void         intensityFactor     (float);
    void         order               (unsigned int);
    void         useLastPosition     (bool);
    void         useIntersection     (bool);
    void         direction           (const glm::vec3&);
    void         useAverageDirection ();

  private:
    IMPLEMENTATION

    void runSculpt (AffectedFaces&, ActionUnitOn <WingedMesh>&) const;
};

#endif
