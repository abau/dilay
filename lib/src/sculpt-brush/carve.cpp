#include "action/unit/on.hpp"
#include "affected-faces.hpp"
#include "intersection.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "primitive/sphere.hpp"
#include "sculpt-brush/carve.hpp"
#include "winged/vertex.hpp"

struct SculptBrushCarve :: Impl {

  SculptBrushCarve* self;
  float             intensityFactor;

  Impl (SculptBrushCarve* s) 
    : self            (s) 
    , intensityFactor (0.0f)
  {}

  float intensity () const {
    return this->intensityFactor * this->self->radius ();
  }

  float sculptDelta (const glm::vec3& v) const {
    const float x = glm::distance <float> (v, this->self->position ());
    if (x >= this->self->radius ())
      return 0.0f;
    else {
      const float n     = 4.0f;
      const float normX = x / this->self->radius ();
      return this->intensity ()
           * (((n-1.0f) * glm::pow (normX, n)) - (n * glm::pow (normX, n-1.0f)) + 1.0f);
    }
  }

  void runSculpt (AffectedFaces& faces, ActionUnitOn <WingedMesh>& actions) const {
    PrimSphere  sphere (this->self->position (), this->self->radius ());
    WingedMesh& mesh   (this->self->meshRef ());

    IntersectionUtil::extend (sphere, mesh, this->self->faceRef (), faces);

    VertexPtrSet vertices (faces.toVertexSet ());

    // get average normal
    glm::vec3 avgNormal (0.0f);
    for (WingedVertex* v : vertices) {
      avgNormal = avgNormal + v->savedNormal (mesh);
    }
    avgNormal = avgNormal / float (vertices.size ());

    // write new positions
    for (WingedVertex* v : vertices) {
      const glm::vec3 oldPos = v->position (mesh);
      const glm::vec3 newPos = oldPos + (avgNormal * this->sculptDelta (oldPos));

      actions.add <PAModifyWVertex> ().move (mesh, *v, newPos);
    }
  }
};

DELEGATE_BIG6_BASE (SculptBrushCarve, (), (this), SculptBrush, ())
  
GETTER_CONST    (float, SculptBrushCarve, intensityFactor)
SETTER          (float, SculptBrushCarve, intensityFactor)
DELEGATE2_CONST (void , SculptBrushCarve, runSculpt, AffectedFaces&, ActionUnitOn <WingedMesh>&)
