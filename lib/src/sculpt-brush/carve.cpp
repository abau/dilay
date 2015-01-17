#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "action/unit/on.hpp"
#include "affected-faces.hpp"
#include "fwd-winged.hpp"
#include "intersection.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "primitive/sphere.hpp"
#include "sculpt-brush/carve.hpp"
#include "winged/vertex.hpp"

struct SculptBrushCarve :: Impl {

  SculptBrushCarve* self;
  float             intensityFactor;
  float             stepWidthFactor;
  glm::vec3         lastPosition;

  Impl (SculptBrushCarve* s) : self (s) {}

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
    WingedMesh& mesh   (this->self->mesh ());

    IntersectionUtil::extend (sphere, mesh, this->self->face (), faces);

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

  float intensity () const {
    return this->intensityFactor * this->self->radius ();
  }

  float stepWidth () const {
    return this->stepWidthFactor * this->self->radius ();
  }

  bool runUpdate (WingedMesh& m, WingedFace& f, const glm::vec3& p) {
    if (this->self->wasUpdated ()) {
      if (glm::distance2 (this->self->position (), p) > this->stepWidth () * this->stepWidth ()) {
        this->lastPosition = this->self->position ();
        this->self->updateBase (m,f,p);
        return true;
      }
      else {
        return false;
      }
    }
    else {
      this->self->updateBase (m,f,p);
      return true;
    }
  }
};

DELEGATE_BIG6_BASE (SculptBrushCarve, (), (this), SculptBrush, ())
  
GETTER_CONST    (float            , SculptBrushCarve, intensityFactor)
GETTER_CONST    (float            , SculptBrushCarve, stepWidthFactor)
GETTER_CONST    (const glm::vec3& , SculptBrushCarve, lastPosition)
SETTER          (float            , SculptBrushCarve, intensityFactor)
SETTER          (float            , SculptBrushCarve, stepWidthFactor)
DELEGATE2_CONST (void             , SculptBrushCarve, runSculpt, AffectedFaces&, ActionUnitOn <WingedMesh>&)
DELEGATE_CONST  (float            , SculptBrushCarve, intensity)
DELEGATE_CONST  (float            , SculptBrushCarve, stepWidth)
DELEGATE3       (bool             , SculptBrushCarve, runUpdate, WingedMesh&, WingedFace&, const glm::vec3&)
