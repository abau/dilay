#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "action/unit/on-winged-mesh.hpp"
#include "affected-faces.hpp"
#include "intersection.hpp"
#include "maybe.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "primitive/sphere.hpp"
#include "sculpt-brush.hpp"
#include "util.hpp"
#include "winged/mesh.hpp"
#include "winged/util.hpp"
#include "winged/vertex.hpp"

struct SculptBrush :: Impl {
  SculptBrush*      self;
  SculptBrush::Mode mode;
  float             radius;
  float             detailFactor;
  float             stepWidthFactor;
  bool              subdivide;
  WingedMesh*       mesh;
  WingedFace*       face;
  bool              hasPosition;
  glm::vec3        _lastPosition;
  glm::vec3        _position;

  float             intensityFactor;
  float             innerRadiusFactor;
  bool              invert;
  Maybe <glm::vec3> mSculptDirection;
  bool              useLastPosition;
  bool              useIntersection;
  bool              linearStep;

  Impl (SculptBrush* s) 
    : self              (s)
    , mode              (SculptBrush::Mode::Nothing)
    , radius            (0.0f)
    , detailFactor      (0.0f)
    , stepWidthFactor   (0.0f)
    , subdivide         (false)
    , hasPosition       (false)

    , intensityFactor   (0.0f)
    , innerRadiusFactor (0.0f)
    , invert            (false)
    , useLastPosition   (false)
    , useIntersection   (false)
    , linearStep        (false)
  {}

  void sculpt (AffectedFaces& faces, ActionUnitOnWMesh& actions) const {
    switch (this->mode) {
      case SculptBrush::Mode::Nothing:   return;
      case SculptBrush::Mode::Translate: return this->sculptTranslate (faces, actions);
      case SculptBrush::Mode::Smooth:    return this->sculptSmooth    (faces, actions);
    }
  }

  void sculptTranslate (AffectedFaces& faces, ActionUnitOnWMesh& actions) const {

    auto getSculptDirection = [this] (const VertexPtrSet& vertices) -> glm::vec3 {
      if (this->mSculptDirection.isSet ()) {
        return this->invert ? -this->mSculptDirection.getRef ()
                            :  this->mSculptDirection.getRef ();
      }
      else {
        const glm::vec3 avgNormal = WingedUtil::averageNormal ( this->self->meshRef ()
                                                              , vertices);
        return this->invert ? -avgNormal
                            :  avgNormal;
      }
    };

    const glm::vec3 position = this->useLastPosition ? this->_lastPosition
                                                     : this->_position;

    float (*stepFunction) (const glm::vec3&, const glm::vec3&, float, float) =
      this->linearStep ? Util::linearStep : Util::smoothStep;

    PrimSphere  sphere (position, this->radius);
    WingedMesh& mesh   (this->self->meshRef ());

    if (this->useIntersection) {
      mesh.intersects (sphere, faces);
    }
    else {
      IntersectionUtil::extend (sphere, mesh, this->self->faceRef (), faces);
    }

    VertexPtrSet    vertices (faces.toVertexSet ());
    const glm::vec3 dir      (getSculptDirection (vertices));

    for (WingedVertex* v : vertices) {
      const glm::vec3 oldPos      = v->position (mesh);
      const float     intensity   = this->intensity   ();
      const float     innerRadius = this->innerRadius ();
      const float     delta       = intensity
                                  * stepFunction ( oldPos, position
                                                 , innerRadius, this->radius );

      const glm::vec3 newPos = oldPos + (delta * dir);

      actions.add <PAModifyWVertex> ().move (mesh, *v, newPos);
    }
  }

  void sculptSmooth (AffectedFaces& faces, ActionUnitOnWMesh&) const {
    PrimSphere sphere (this->_position, this->radius);

    IntersectionUtil::extend ( sphere, this->self->meshRef ()
                             , this->self->faceRef (), faces );
  }

  float intensity () const {
    return this->intensityFactor * this->radius;
  }

  float innerRadius () const {
    return this->innerRadiusFactor * this->radius;
  }

  float subdivThreshold () const {
    return (1.0f - this->detailFactor) * this->radius;
  }

  const glm::vec3& lastPosition () const {
    assert (this->hasPosition);
    return this->_lastPosition;
  }

  const glm::vec3& position () const {
    assert (this->hasPosition);
    return this->_position;
  }

  glm::vec3 delta () const {
    assert (this->hasPosition);
    return this->_position - this->_lastPosition;
  }

  void setPosition (const glm::vec3& p) {
    this->hasPosition   = true;
    this->_lastPosition = p;
    this->_position     = p;
  }

  bool updateDelta (const glm::vec3& delta) {
    assert (this->hasPosition);

    const float stepWidth = this->stepWidthFactor * this->self->radius ();

    if (glm::length2 (delta) > stepWidth * stepWidth) {
      this->_lastPosition = this->_position;
      this->_position     = this->_position + delta;
      return true;
    }
    else {
      return false;
    }
  }

  bool updatePosition (const glm::vec3& p) {
    if (this->hasPosition) {
      return this->updateDelta (p - this->_position);
    }
    else {
      this->setPosition (p);
      return true;
    }
  }

  void resetPosition () {
    this->hasPosition = false;
  }

  void toggleInvert () {
    this->invert = not this->invert;
  }

  void direction (const glm::vec3& d) {
    this->mSculptDirection = d;
  }

  void useAverageDirection () {
    this->mSculptDirection.reset ();
  }
};

DELEGATE_BIG6_SELF (SculptBrush)
  
DELEGATE2_CONST (void             , SculptBrush, sculpt, AffectedFaces&, ActionUnitOnWMesh&)
GETTER_CONST    (SculptBrush::Mode, SculptBrush, mode)
GETTER_CONST    (float            , SculptBrush, radius)
GETTER_CONST    (float            , SculptBrush, detailFactor)
GETTER_CONST    (float            , SculptBrush, stepWidthFactor)
GETTER_CONST    (bool             , SculptBrush, subdivide)
GETTER_CONST    (WingedMesh*      , SculptBrush, mesh)
GETTER_CONST    (WingedFace*      , SculptBrush, face)
SETTER          (SculptBrush::Mode, SculptBrush, mode)
SETTER          (float            , SculptBrush, radius)
SETTER          (float            , SculptBrush, detailFactor)
SETTER          (float            , SculptBrush, stepWidthFactor)
SETTER          (bool             , SculptBrush, subdivide)
SETTER          (WingedMesh*      , SculptBrush, mesh)
SETTER          (WingedFace*      , SculptBrush, face)
DELEGATE_CONST  (float            , SculptBrush, subdivThreshold)
GETTER_CONST    (bool             , SculptBrush, hasPosition)
DELEGATE_CONST  (const glm::vec3& , SculptBrush, lastPosition)
DELEGATE_CONST  (const glm::vec3& , SculptBrush, position)
DELEGATE_CONST  (glm::vec3        , SculptBrush, delta)
DELEGATE1       (void             , SculptBrush, setPosition, const glm::vec3&)
DELEGATE1       (bool             , SculptBrush, updateDelta, const glm::vec3&)
DELEGATE1       (bool             , SculptBrush, updatePosition, const glm::vec3&)
DELEGATE        (void             , SculptBrush, resetPosition)
GETTER_CONST    (float            , SculptBrush, intensityFactor)
GETTER_CONST    (float            , SculptBrush, innerRadiusFactor)
GETTER_CONST    (bool             , SculptBrush, invert)
SETTER          (float            , SculptBrush, intensityFactor)
SETTER          (float            , SculptBrush, innerRadiusFactor)
SETTER          (bool             , SculptBrush, invert)
DELEGATE        (void             , SculptBrush, toggleInvert)
SETTER          (bool             , SculptBrush, useLastPosition)
SETTER          (bool             , SculptBrush, useIntersection)
DELEGATE1       (void             , SculptBrush, direction, const glm::vec3&)
DELEGATE        (void             , SculptBrush, useAverageDirection)
SETTER          (bool             , SculptBrush, linearStep)
