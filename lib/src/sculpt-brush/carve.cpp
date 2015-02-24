#include "action/unit/on.hpp"
#include "affected-faces.hpp"
#include "intersection.hpp"
#include "maybe.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "primitive/plane.hpp"
#include "primitive/sphere.hpp"
#include "sculpt-brush/carve.hpp"
#include "sculpt-brush/util.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

struct SculptBrushCarve :: Impl {

  SculptBrushCarve* self;
  float             intensityFactor;
  unsigned int      flatness;
  bool              invert;
  Maybe <glm::vec3> mDirection;
  bool              useLastPosition;
  bool              useIntersection;
  bool              carvePerimeter;

  Impl (SculptBrushCarve* s) 
    : self            (s) 
    , intensityFactor (0.0f)
    , flatness        (4)
    , invert          (false)
    , useLastPosition (false)
    , useIntersection (false)
    , carvePerimeter  (false)
  {}

  float intensity () const {
    assert (this->intensityFactor >= 0.0f);

    return this->intensityFactor * this->self->radius ();
  }

  unsigned int boundedFlatness () const {
    return this->flatness < 3 ? 3 : this->flatness;
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
      glm::vec3 avgNormal (0.0f);
      for (WingedVertex* v : vertices) {
        avgNormal = avgNormal + v->savedNormal (mesh);
      }
      avgNormal = avgNormal / float (vertices.size ());
      return this->invert ? -avgNormal
                          :  avgNormal;
    }
  }

  glm::vec3 getPosition () const {
    return this->useLastPosition ? this->self->lastPosition ()
                                 : this->self->position     ();
  }

  void runCarveCenter (VertexPtrSet& vertices, ActionUnitOn <WingedMesh>& actions) const {
          WingedMesh& mesh (this->self->meshRef ());
    const glm::vec3   dir  (this->getDirection (mesh, vertices));

    for (WingedVertex* v : vertices) {
      const glm::vec3 oldPos = v->position (mesh);
      const float     delta  = this->intensity ()
                             * SculptBrushUtil::smooth ( oldPos, this->getPosition ()
                                                       , this->self->radius ()
                                                       , this->boundedFlatness () );
      const glm::vec3 newPos = oldPos + (delta * dir);

      actions.add <PAModifyWVertex> ().move (mesh, *v, newPos);
    }
  }

  void runCarvePerimeter (VertexPtrSet& vertices, ActionUnitOn <WingedMesh>& actions) const {
          WingedMesh& mesh  (this->self->meshRef ());
    const glm::vec3   dir   (this->getDirection (mesh, vertices));
    const PrimPlane   plane (this->self->position (), dir);

    for (WingedVertex* v : vertices) {
      const glm::vec3 oldPos  = v->position (mesh);
      const glm::vec3 onPlane = oldPos - (dir * plane.distance (oldPos));
      const float     delta   = SculptBrushUtil::smooth ( onPlane
                                                        , this->self->position ()
                                                        , this->self->radius ()
                                                        , this->boundedFlatness () );
      const glm::vec3 newPos  = oldPos + (delta * (onPlane - oldPos));

      actions.add <PAModifyWVertex> ().move (mesh, *v, newPos);
    }
  }

  void runSculpt (AffectedFaces& faces, ActionUnitOn <WingedMesh>& actions) const {
    PrimSphere sphere (this->getPosition (), this->self->radius ());

    if (this->useIntersection) {
      this->self->meshRef ().intersects (sphere, faces);
    }
    else {
      IntersectionUtil::extend ( sphere, this->self->meshRef ()
                               , this->self->faceRef (), faces );
    }
    VertexPtrSet vertices (faces.toVertexSet ());

    if (this->carvePerimeter) {
      this->runCarvePerimeter (vertices, actions);
    }
    else {
      this->runCarveCenter (vertices, actions);
    }
  }
};

DELEGATE_BIG6_BASE (SculptBrushCarve, (), (this), SculptBrush, ())
  
GETTER_CONST    (float        , SculptBrushCarve, intensityFactor)
GETTER_CONST    (unsigned int , SculptBrushCarve, flatness)
GETTER_CONST    (bool         , SculptBrushCarve, invert)
GETTER_CONST    (bool         , SculptBrushCarve, carvePerimeter)
SETTER          (float        , SculptBrushCarve, intensityFactor)
SETTER          (unsigned int , SculptBrushCarve, flatness)
SETTER          (bool         , SculptBrushCarve, invert)
DELEGATE        (void         , SculptBrushCarve, toggleInvert)
SETTER          (bool         , SculptBrushCarve, carvePerimeter)
SETTER          (bool         , SculptBrushCarve, useLastPosition)
SETTER          (bool         , SculptBrushCarve, useIntersection)
DELEGATE1       (void         , SculptBrushCarve, direction, const glm::vec3&)
DELEGATE        (void         , SculptBrushCarve, useAverageDirection)
DELEGATE2_CONST (void         , SculptBrushCarve, runSculpt, AffectedFaces&, ActionUnitOn <WingedMesh>&)
