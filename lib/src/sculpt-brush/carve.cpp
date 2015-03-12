#include "action/unit/on-winged-mesh.hpp"
#include "affected-faces.hpp"
#include "intersection.hpp"
#include "maybe.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "primitive/plane.hpp"
#include "primitive/sphere.hpp"
#include "sculpt-brush/carve.hpp"
#include "winged/mesh.hpp"
#include "winged/util.hpp"
#include "winged/vertex.hpp"
#include "util.hpp"

struct SculptBrushCarve :: Impl {

  SculptBrushCarve* self;
  float             intensityFactor;
  float             innerRadiusFactor;
  bool              invert;
  Maybe <glm::vec3> mDirection;
  bool              useLastPosition;
  bool              useIntersection;

  Impl (SculptBrushCarve* s) 
    : self              (s) 
    , intensityFactor   (0.0f)
    , innerRadiusFactor (0.5f)
    , invert            (false)
    , useLastPosition   (false)
    , useIntersection   (false)
  {}

  float intensity () const {
    assert (this->intensityFactor >= 0.0f);

    return this->intensityFactor * this->self->radius ();
  }

  float innerRadius () const {
    assert (this->innerRadiusFactor >= 0.0f);
    assert (this->innerRadiusFactor <= 1.0f);

    return this->innerRadiusFactor * this->self->radius ();
  }

  void toggleInvert () {
    this->invert = not this->invert;
  }

  void direction (const glm::vec3& d) {
    this->mDirection = d;
  }

  void useAverageDirection () {
    this->mDirection.reset ();
  }

  glm::vec3 getDirection (const WingedMesh& mesh, const VertexPtrSet& vertices) const {
    if (this->mDirection.isSet ()) {
      return this->invert ? -this->mDirection.getRef ()
                          :  this->mDirection.getRef ();
    }
    else {
      const glm::vec3 avgNormal = WingedUtil::averageNormal (mesh, vertices);
      return this->invert ? -avgNormal
                          :  avgNormal;
    }
  }

  glm::vec3 getPosition () const {
    return this->useLastPosition ? this->self->lastPosition ()
                                 : this->self->position     ();
  }

  void runSculpt (AffectedFaces& faces, ActionUnitOnWMesh& actions) const {
    PrimSphere  sphere (this->getPosition (), this->self->radius ());
    WingedMesh& mesh   (this->self->meshRef ());

    if (this->useIntersection) {
      this->self->meshRef ().intersects (sphere, faces);
    }
    else {
      IntersectionUtil::extend ( sphere, this->self->meshRef ()
                               , this->self->faceRef (), faces );
    }
    VertexPtrSet    vertices (faces.toVertexSet ());
    const glm::vec3 dir      (this->getDirection (mesh, vertices));

    for (WingedVertex* v : vertices) {
      const glm::vec3 oldPos = v->position (mesh);
      const float     delta  = this->intensity ()
                             * Util::smoothStep 
                                 ( oldPos, this->getPosition ()
                                 , this->innerRadius ()
                                 , this->self->radius () );
      const glm::vec3 newPos = oldPos + (delta * dir);

      actions.add <PAModifyWVertex> ().move (mesh, *v, newPos);
    }
  }
};

DELEGATE_BIG6_BASE (SculptBrushCarve, (), (this), SculptBrush, ())
  
GETTER_CONST    (float, SculptBrushCarve, intensityFactor)
GETTER_CONST    (float, SculptBrushCarve, innerRadiusFactor)
GETTER_CONST    (bool , SculptBrushCarve, invert)
SETTER          (float, SculptBrushCarve, intensityFactor)
SETTER          (float, SculptBrushCarve, innerRadiusFactor)
SETTER          (bool , SculptBrushCarve, invert)
DELEGATE        (void , SculptBrushCarve, toggleInvert)
SETTER          (bool , SculptBrushCarve, useLastPosition)
SETTER          (bool , SculptBrushCarve, useIntersection)
DELEGATE1       (void , SculptBrushCarve, direction, const glm::vec3&)
DELEGATE        (void , SculptBrushCarve, useAverageDirection)
DELEGATE2_CONST (void , SculptBrushCarve, runSculpt, AffectedFaces&, ActionUnitOnWMesh&)
