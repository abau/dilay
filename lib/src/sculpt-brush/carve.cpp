#include "action/unit/on.hpp"
#include "affected-faces.hpp"
#include "intersection.hpp"
#include "maybe.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "primitive/sphere.hpp"
#include "sculpt-brush/carve.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

struct SculptBrushCarve :: Impl {

  SculptBrushCarve* self;
  float             intensityFactor;
  unsigned int      order;
  Maybe <glm::vec3> mDirection;
  bool              useLastPosition;
  bool              useIntersection;

  Impl (SculptBrushCarve* s) 
    : self            (s) 
    , intensityFactor (0.0f)
    , order           (4)
    , useLastPosition (false)
    , useIntersection (false)
  {}

  float intensity () const {
    return this->intensityFactor * this->self->radius ();
  }

  void direction (const glm::vec3& d) {
    this->mDirection = d;
  }

  void useAverageDirection () {
    this->mDirection.reset ();
  }

  float sculptDelta (const glm::vec3& v) const {
    assert (this->order > 2);

    const float x = glm::distance <float> (v, this->getPosition ());
    if (x >= this->self->radius ())
      return 0.0f;
    else {
      const float normX = x / this->self->radius ();
      return this->intensity ()
           * ( ((this->order-1.0f) * glm::pow (normX, this->order))
             - (this->order * glm::pow (normX, this->order-1.0f)) 
             + 1.0f );
    }
  }

  glm::vec3 getDirection (const WingedMesh& mesh, const VertexPtrSet& vertices) const {
    if (this->mDirection.isSet ()) {
      return this->mDirection.getRef ();
    }
    else {
      glm::vec3 avgNormal (0.0f);
      for (WingedVertex* v : vertices) {
        avgNormal = avgNormal + v->savedNormal (mesh);
      }
      avgNormal = avgNormal / float (vertices.size ());
      return avgNormal;
    }
  }

  glm::vec3 getPosition () const {
    return this->useLastPosition ? this->self->lastPosition ()
                                 : this->self->position     ();
  }

  void runSculpt (AffectedFaces& faces, ActionUnitOn <WingedMesh>& actions) const {
    PrimSphere  sphere (this->getPosition (), this->self->radius ());
    WingedMesh& mesh   (this->self->meshRef ());

    if (this->useIntersection) {
      mesh.intersects (sphere, faces);
    }
    else {
      IntersectionUtil::extend (sphere, mesh, this->self->faceRef (), faces);
    }

    VertexPtrSet vertices (faces.toVertexSet ());
    const glm::vec3 d = this->getDirection (mesh, vertices);

    for (WingedVertex* v : vertices) {
      const glm::vec3 oldPos = v->position (mesh);
      const glm::vec3 newPos = oldPos + (d * this->sculptDelta (oldPos));

      actions.add <PAModifyWVertex> ().move (mesh, *v, newPos);
    }
  }
};

DELEGATE_BIG6_BASE (SculptBrushCarve, (), (this), SculptBrush, ())
  
GETTER_CONST    (float        , SculptBrushCarve, intensityFactor)
GETTER_CONST    (unsigned int , SculptBrushCarve, order)
SETTER          (float        , SculptBrushCarve, intensityFactor)
SETTER          (unsigned int , SculptBrushCarve, order)
SETTER          (bool         , SculptBrushCarve, useLastPosition)
SETTER          (bool         , SculptBrushCarve, useIntersection)
DELEGATE1       (void         , SculptBrushCarve, direction, const glm::vec3&)
DELEGATE        (void         , SculptBrushCarve, useAverageDirection)
DELEGATE2_CONST (void         , SculptBrushCarve, runSculpt, AffectedFaces&, ActionUnitOn <WingedMesh>&)
