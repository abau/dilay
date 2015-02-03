#include "primitive/sphere.hpp"
#include "sculpt-brush/subdivide.hpp"
#include "winged/mesh.hpp"

struct SculptBrushSubdivide :: Impl {
  SculptBrushSubdivide* self;

  Impl (SculptBrushSubdivide* s) : self (s) {}

  void runSculpt (AffectedFaces& faces, ActionUnitOn <WingedMesh>&) const {
    PrimSphere  sphere (this->self->position (), this->self->radius ());

    this->self->meshRef ().intersects (sphere, faces);
  }
};

DELEGATE_BIG6_BASE (SculptBrushSubdivide, (), (this), SculptBrush, ())
DELEGATE2_CONST (void , SculptBrushSubdivide, runSculpt, AffectedFaces&, ActionUnitOn <WingedMesh>&)
