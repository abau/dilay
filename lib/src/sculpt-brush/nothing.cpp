#include "intersection.hpp"
#include "primitive/sphere.hpp"
#include "sculpt-brush/nothing.hpp"

struct SculptBrushNothing :: Impl {
  SculptBrushNothing* self;

  Impl (SculptBrushNothing* s) : self (s) {}

  void runSculpt (AffectedFaces& faces, ActionUnitOnWMesh&) const {
    const PrimSphere sphere (this->self->position (), this->self->radius ());

    IntersectionUtil::extend (sphere, this->self->meshRef (), this->self->faceRef (), faces);
  }
};

DELEGATE_BIG6_BASE (SculptBrushNothing, (), (this), SculptBrush, ())
DELEGATE2_CONST (void, SculptBrushNothing, runSculpt, AffectedFaces&, ActionUnitOnWMesh&)
