/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/gtx/norm.hpp>
#include "affected-faces.hpp"
#include "intersection.hpp"
#include "primitive/plane.hpp"
#include "primitive/sphere.hpp"
#include "sculpt-brush.hpp"
#include "util.hpp"
#include "winged/mesh.hpp"
#include "winged/util.hpp"
#include "winged/vertex.hpp"
#include "variant.hpp"

SBIntensityParameters :: SBIntensityParameters ()
  : _intensity (0.0f)
{}

SBInvertParameters :: SBInvertParameters ()
  : _invert (false)
{}

glm::vec3 SBInvertParameters :: invert (const glm::vec3& v) const {
  return this->_invert ? -v : v;
}

SBCarveParameters :: SBCarveParameters ()
  : _inflate (false)
{}

SBDraglikeParameters :: SBDraglikeParameters ()
  : _smoothness       (1.0f)
  , _discardBackfaces (true)
  , _linearStep       (false)
{}

SBSmoothParameters :: SBSmoothParameters ()
  : _relaxOnly (false)
{}

struct SculptBrush :: Impl {
  SculptBrush*  self;
  float         radius;
  float         detailFactor;
  float         stepWidthFactor;
  bool          subdivide;
  WingedMesh*   mesh;
  bool          hasPosition;
  glm::vec3    _lastPosition;
  glm::vec3    _position;
  glm::vec3    _direction;

  Variant < SBCarveParameters
          , SBDraglikeParameters
          , SBSmoothParameters
          , SBFlattenParameters
          , SBCreaseParameters
          , SBPinchParameters
          , SBReduceParameters > parameters;

  Impl (SculptBrush* s) 
    : self            (s)
    , radius          (0.0f)
    , detailFactor    (0.0f)
    , stepWidthFactor (0.0f)
    , subdivide       (false)
    , hasPosition     (false)
  {}

  void sculpt (AffectedFaces& faces) const {
    assert (this->parameters.isSet ());

    this->parameters.caseOf <void>
      ( [this,&faces] (const SBCarveParameters&    p) { this->sculpt (p, faces); }
      , [this,&faces] (const SBDraglikeParameters& p) { this->sculpt (p, faces); }
      , [this,&faces] (const SBSmoothParameters&   p) { this->sculpt (p, faces); }
      , [this,&faces] (const SBFlattenParameters&  p) { this->sculpt (p, faces); }
      , [this,&faces] (const SBCreaseParameters&   p) { this->sculpt (p, faces); }
      , [this,&faces] (const SBPinchParameters&    p) { this->sculpt (p, faces); }
      , [this,&faces] (const SBReduceParameters&   p) { this->sculpt (p, faces); }
      );
  }

  void sculpt (const SBCarveParameters& parameters, AffectedFaces& faces) const {
    PrimSphere  sphere (this->position (), this->radius);
    WingedMesh& mesh   (this->self->meshRef ());

    mesh.intersects (sphere, faces);
    faces.discardBackfaces (mesh, this->direction ());

    if (faces.isEmpty () == false) {
      VertexPtrSet vertices (faces.toVertexSet ());

      const glm::vec3 avgDir ( parameters.inflate ()
                             ? glm::vec3 (0.0f)
                             : parameters.invert (WingedUtil::averageNormal (mesh, vertices)) );

      for (WingedVertex* v : vertices) {
        const glm::vec3 oldPos    = v->position (mesh);
        const float     intensity = parameters.intensity () * this->radius;
        const float     factor    = intensity
                                  * Util::smoothStep ( oldPos, this->position ()
                                                     , 0.0f, this->radius );
        const glm::vec3 direction = parameters.inflate ()
                                  ? parameters.invert (v->savedNormal (mesh))
                                  : avgDir;
        const glm::vec3 newPos    = oldPos + (factor * direction);

        v->writePosition (mesh, newPos);
      }
    }
  }

  void sculpt (const SBDraglikeParameters& parameters, AffectedFaces& faces) const {
    PrimSphere  sphere (this->lastPosition (), this->radius);
    WingedMesh& mesh   (this->self->meshRef ());

    mesh.intersects (sphere, faces);

    if (parameters.discardBackfaces ()) {
      faces.discardBackfaces (mesh, this->direction ());
    }

    if (faces.isEmpty () == false) {
      VertexPtrSet vertices (faces.toVertexSet ());

      float (*stepFunction) (const glm::vec3&, const glm::vec3&, float, float) =
        parameters.linearStep () ? Util::linearStep : Util::smoothStep;

      for (WingedVertex* v : vertices) {
        const glm::vec3 oldPos      = v->position (mesh);
        const float     innerRadius = (1.0f - parameters.smoothness ()) * this->radius;
        const float     factor      = stepFunction ( oldPos, this->lastPosition ()
                                                   , innerRadius, this->radius );
        const glm::vec3 newPos      = oldPos + (factor * this->direction ());

        v->writePosition (mesh, newPos);
      }
    }
  }

  void sculpt (const SBSmoothParameters& parameters, AffectedFaces& faces) const {
    PrimSphere  sphere (this->_position, this->radius);
    WingedMesh& mesh   (this->self->meshRef ());

    mesh .intersects (sphere, faces);
    faces.discardBackfaces (mesh, this->direction ());

    if (faces.isEmpty () == false && parameters.relaxOnly () == false) {
      VertexPtrSet vertices (faces.toVertexSet ());

      for (WingedVertex* v : vertices) {
        const glm::vec3 oldPos = v->position (mesh);
        const float     factor = parameters.intensity ()
                               * Util::smoothStep ( oldPos, this->position ()
                                                  , 0.0f, this->radius);
        const glm::vec3 newPos = oldPos + (factor * (WingedUtil::center (mesh, *v) - oldPos));

        v->writePosition (mesh, newPos);
      }
    }
  }

  void sculpt (const SBFlattenParameters& parameters, AffectedFaces& faces) const {
    PrimSphere  sphere (this->_position, this->radius);
    WingedMesh& mesh   (this->self->meshRef ());

    mesh.intersects (sphere, faces);
    faces.discardBackfaces (mesh, this->direction ());

    if (faces.isEmpty () == false) {
      VertexPtrSet    vertices (faces.toVertexSet ());
      const glm::vec3 normal   (WingedUtil::averageNormal (mesh, vertices));
      const PrimPlane plane    (WingedUtil::center (mesh, vertices), normal);

      for (WingedVertex* v : vertices) {
        const glm::vec3 oldPos   = v->position (mesh);
        const float     factor   = parameters.intensity ()
                                 * Util::linearStep ( oldPos, this->position ()
                                                    , 0.0f, this->radius );
        const float     distance = glm::max (0.0f, plane.distance (oldPos));
        const glm::vec3 newPos   = oldPos - (normal * factor * distance);

        v->writePosition (mesh, newPos);
      }
    }
  }

  void sculpt (const SBCreaseParameters& parameters, AffectedFaces& faces) const {
    PrimSphere  sphere (this->position (), this->radius);
    WingedMesh& mesh   (this->self->meshRef ());

    mesh.intersects (sphere, faces);

    faces.discardBackfaces (mesh, this->direction ());

    if (faces.isEmpty () == false) {
      VertexPtrSet    vertices (faces.toVertexSet ());
      const glm::vec3 avgDir   (parameters.invert (WingedUtil::averageNormal (mesh, vertices)));
      const glm::vec3 refPos   (this->position () + (avgDir * parameters.intensity () * this->radius));
      const PrimPlane plane    (refPos, avgDir);

      for (WingedVertex* v : vertices) {
        const glm::vec3 oldPos   = v->position (mesh);
        const glm::vec3 projPos  = plane.project (oldPos);
        const float     distance = glm::distance (projPos, refPos);

        if (glm::distance (projPos, refPos) > 0.001f) {
          const float     relDistance = glm::clamp (distance / this->radius, 0.0f, 1.0f);
          const float     factor      = 0.1f * this->radius * glm::min (0.5f, 1.0f - relDistance);
          const glm::vec3 direction   = glm::normalize ( (projPos - oldPos)
                                                       + (2.0f * (refPos - projPos)) );
          const glm::vec3 newPos      = oldPos + (factor * direction);

          v->writePosition (mesh, newPos);
        }
      }
    }
  }

  void sculpt (const SBPinchParameters& parameters, AffectedFaces& faces) const {
    PrimSphere  sphere (this->position (), this->radius);
    WingedMesh& mesh   (this->self->meshRef ());

    mesh.intersects (sphere, faces);

    faces.discardBackfaces (mesh, this->direction ());

    if (faces.isEmpty () == false) {
      VertexPtrSet    vertices (faces.toVertexSet ());

      for (WingedVertex* v : vertices) {
        const glm::vec3 oldPos   = v->position (mesh);
        const float     distance = glm::distance (oldPos, this->position ());

        if (distance > 0.001f) {
          const float     relDistance = glm::clamp (distance / this->radius, 0.0f, 1.0f);
          const float     factor      = 0.1f * this->radius * glm::min (0.5f, 1.0f - relDistance);
          const glm::vec3 direction   = parameters.invert (glm::normalize (this->position () - oldPos));
          const glm::vec3 newPos      = oldPos + (factor * direction);

          v->writePosition (mesh, newPos);
        }
      }
    }
  }

  void sculpt (const SBReduceParameters&, AffectedFaces& faces) const {
    this->self->meshRef ().intersects (PrimSphere (this->position (), this->radius), faces);
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

  const glm::vec3& direction () const {
    assert (this->hasPosition);
    return this->_direction;
  }

  glm::vec3 delta () const {
    assert (this->hasPosition);
    return this->_position - this->_lastPosition;
  }

  void setPointOfAction (const glm::vec3& p, const glm::vec3& d) {
    this->hasPosition   = true;
    this->_lastPosition = p;
    this->_position     = p;
    this->_direction    = d;
  }

  bool updatePointOfAction (const glm::vec3& p, const glm::vec3& d) {
    if (this->hasPosition) {
      const float stepWidth = this->stepWidthFactor * glm::log (this->self->radius () + 1);

      if (glm::distance2 (p, this->_position) > stepWidth * stepWidth) {
        this->_lastPosition = this->_position;
        this->_position     = p;
        this->_direction    = d;
        return true;
      }
      else {
        return false;
      }
    }
    else {
      this->setPointOfAction (p, d);
      return true;
    }
  }

  void resetPointOfAction () {
    this->hasPosition = false;
  }

  bool reduce () const {
    return this->parameters.is <SBReduceParameters> ();
  }

  void mirror (const PrimPlane& plane) {
    if (this->hasPosition) {
      this->_lastPosition = plane.mirror          (this->_lastPosition);
      this->_position     = plane.mirror          (this->_position);
      this->_direction    = plane.mirrorDirection (this->_direction);
    }
  }
};

DELEGATE_BIG6_SELF (SculptBrush)
  
DELEGATE1_CONST (void             , SculptBrush, sculpt, AffectedFaces&)
GETTER_CONST    (float            , SculptBrush, radius)
GETTER_CONST    (float            , SculptBrush, detailFactor)
GETTER_CONST    (float            , SculptBrush, stepWidthFactor)
GETTER_CONST    (bool             , SculptBrush, subdivide)
GETTER_CONST    (WingedMesh*      , SculptBrush, mesh)
SETTER          (float            , SculptBrush, radius)
SETTER          (float            , SculptBrush, detailFactor)
SETTER          (float            , SculptBrush, stepWidthFactor)
SETTER          (bool             , SculptBrush, subdivide)
SETTER          (WingedMesh*      , SculptBrush, mesh)
DELEGATE_CONST  (float            , SculptBrush, subdivThreshold)
GETTER_CONST    (bool             , SculptBrush, hasPosition)
DELEGATE_CONST  (const glm::vec3& , SculptBrush, lastPosition)
DELEGATE_CONST  (const glm::vec3& , SculptBrush, position)
DELEGATE_CONST  (const glm::vec3& , SculptBrush, direction)
DELEGATE_CONST  (glm::vec3        , SculptBrush, delta)
DELEGATE2       (void             , SculptBrush, setPointOfAction, const glm::vec3&, const glm::vec3&)
DELEGATE2       (bool             , SculptBrush, updatePointOfAction, const glm::vec3&, const glm::vec3&)
DELEGATE        (void             , SculptBrush, resetPointOfAction)
DELEGATE_CONST  (bool             , SculptBrush, reduce)
DELEGATE1       (void             , SculptBrush, mirror, const PrimPlane&)

template <typename T> 
const T& SculptBrush::constParameters () const {
  return this->impl->parameters.get <T> ();
}


template <typename T>
T& SculptBrush::parameters () {
  if (this->impl->parameters.is <T> () == false) {
    this->impl->parameters.init <T> ();
  }
  return this->impl->parameters.get <T> ();
}

template const SBCarveParameters& SculptBrush::constParameters <SBCarveParameters> () const;
template       SBCarveParameters& SculptBrush::parameters      <SBCarveParameters> ();

template const SBDraglikeParameters& SculptBrush::constParameters <SBDraglikeParameters> () const;
template       SBDraglikeParameters& SculptBrush::parameters      <SBDraglikeParameters> ();

template const SBSmoothParameters& SculptBrush::constParameters <SBSmoothParameters> () const;
template       SBSmoothParameters& SculptBrush::parameters      <SBSmoothParameters> ();

template const SBFlattenParameters& SculptBrush::constParameters <SBFlattenParameters> () const;
template       SBFlattenParameters& SculptBrush::parameters      <SBFlattenParameters> ();

template const SBCreaseParameters& SculptBrush::constParameters <SBCreaseParameters> () const;
template       SBCreaseParameters& SculptBrush::parameters      <SBCreaseParameters> ();

template const SBPinchParameters& SculptBrush::constParameters <SBPinchParameters> () const;
template       SBPinchParameters& SculptBrush::parameters      <SBPinchParameters> ();

template const SBReduceParameters& SculptBrush::constParameters <SBReduceParameters> () const;
template       SBReduceParameters& SculptBrush::parameters      <SBReduceParameters> ();
