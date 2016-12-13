/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/gtx/norm.hpp>
#include <memory>
#include "adjacent-iterator.hpp"
#include "affected-faces.hpp"
#include "intersection.hpp"
#include "primitive/plane.hpp"
#include "primitive/sphere.hpp"
#include "primitive/triangle.hpp"
#include "sculpt-brush.hpp"
#include "util.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/util.hpp"
#include "winged/vertex.hpp"

void SBCarveParameters :: sculpt (const SculptBrush& brush, const VertexPtrSet& vertices) const {
  WingedMesh& mesh = brush.meshRef ();

  if (vertices.empty () == false) {
    const glm::vec3 avgDir = this->inflate ()
                           ? glm::vec3 (0.0f)
                           : this->invert (WingedUtil::averageNormal (mesh, vertices));

    for (WingedVertex* v : vertices) {
      const glm::vec3 oldPos    = v->position (mesh);
      const float     intensity = this->intensity () * brush.radius ();
      const float     factor    = intensity
                                * Util::smoothStep ( oldPos, brush.position ()
                                                   , 0.0f, brush.radius () );
      const glm::vec3 direction = this->inflate ()
                                ? this->invert (v->savedNormal (mesh))
                                : avgDir;
      const glm::vec3 newPos    = oldPos + (factor * direction);

      v->setPosition (mesh, newPos);
    }
  }
}

void SBDraglikeParameters :: sculpt (const SculptBrush& brush, const VertexPtrSet& vertices) const {
  WingedMesh& mesh = brush.meshRef ();

  float (*stepFunction) (const glm::vec3&, const glm::vec3&, float, float) =
    this->linearStep () ? Util::linearStep : Util::smoothStep;

  for (WingedVertex* v : vertices) {
    const glm::vec3 oldPos      = v->position (mesh);
    const float     innerRadius = (1.0f - this->smoothness ()) * brush.radius ();
    const float     factor      = stepFunction ( oldPos, brush.lastPosition ()
                                               , innerRadius, brush.radius () );
    const glm::vec3 newPos      = oldPos + (factor * brush.direction ());

    v->setPosition (mesh, newPos);
  }
}

void SBSmoothParameters :: sculpt (const SculptBrush& brush, const VertexPtrSet& vertices) const {
  WingedMesh& mesh = brush.meshRef ();

  if (this->relaxOnly () == false) {
    for (WingedVertex* v : vertices) {
      const glm::vec3 oldPos = v->position (mesh);
      const float     factor = this->intensity ()
                             * Util::smoothStep ( oldPos, brush.position ()
                                                , 0.0f, brush.radius () );
      const glm::vec3 newPos = oldPos + (factor * (WingedUtil::center (mesh, *v) - oldPos));

      v->setPosition (mesh, newPos);
    }
  }
}

void SBReduceParameters :: sculpt (const SculptBrush&, const VertexPtrSet&) const {}

void SBFlattenParameters :: sculpt (const SculptBrush& brush, const VertexPtrSet& vertices) const {
  WingedMesh& mesh = brush.meshRef ();

  if (vertices.empty () == false) {
    const glm::vec3 normal = WingedUtil::averageNormal (mesh, vertices);
    const PrimPlane plane (WingedUtil::center (mesh, vertices), normal);

    for (WingedVertex* v : vertices) {
      const glm::vec3 oldPos   = v->position (mesh);
      const float     factor   = this->intensity ()
                               * Util::linearStep ( oldPos, brush.position ()
                                                  , 0.0f, brush.radius () );
      const float     distance = glm::max (0.0f, plane.distance (oldPos));
      const glm::vec3 newPos   = oldPos - (normal * factor * distance);

      v->setPosition (mesh, newPos);
    }
  }
}

void SBCreaseParameters :: sculpt (const SculptBrush& brush, const VertexPtrSet& vertices) const {
  WingedMesh& mesh = brush.meshRef ();

  if (vertices.empty () == false) {
    const glm::vec3 avgDir = this->invert (WingedUtil::averageNormal (mesh, vertices));
    const glm::vec3 refPos = brush.position () + (avgDir * this->intensity () * brush.radius ());

    const PrimPlane plane (refPos, avgDir);

    for (WingedVertex* v : vertices) {
      const glm::vec3 oldPos   = v->position (mesh);
      const glm::vec3 projPos  = plane.project (oldPos);
      const float     distance = glm::distance (projPos, refPos);

      if (glm::distance (projPos, refPos) > 0.001f) {
        const float     relDistance = glm::clamp (distance / brush.radius (), 0.0f, 1.0f);
        const float     factor      = 0.1f * brush.radius () * glm::min (0.5f, 1.0f - relDistance);
        const glm::vec3 direction   = glm::normalize ( (projPos - oldPos)
                                                     + (2.0f * (refPos - projPos)) );
        const glm::vec3 newPos      = oldPos + (factor * direction);

        v->setPosition (mesh, newPos);
      }
    }
  }
}

void SBPinchParameters :: sculpt (const SculptBrush& brush, const VertexPtrSet& vertices) const {
  WingedMesh& mesh = brush.meshRef ();

  for (WingedVertex* v : vertices) {
    const glm::vec3 oldPos   = v->position (mesh);
    const float     distance = glm::distance (oldPos, brush.position ());

    if (distance > 0.001f) {
      const float     relDistance = glm::clamp (distance / brush.radius (), 0.0f, 1.0f);
      const float     factor      = 0.1f * brush.radius () * glm::min (0.5f, 1.0f - relDistance);
      const glm::vec3 direction   = this->invert (glm::normalize (brush.position () - oldPos));
      const glm::vec3 newPos      = oldPos + (factor * direction);

      v->setPosition (mesh, newPos);
    }
  }
}

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

  std::unique_ptr <SBParameters> _parameters;

  Impl (SculptBrush* s) 
    : self            (s)
    , radius          (0.0f)
    , detailFactor    (0.0f)
    , stepWidthFactor (0.0f)
    , subdivide       (false)
    , hasPosition     (false)
  {}

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

  PrimSphere sphere () const {
    const glm::vec3& pos = this->_parameters->useLastPos () ? this->lastPosition ()
                                                            : this->position ();
    return PrimSphere (pos, this->radius);
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

  void mirror (const PrimPlane& plane) {
    if (this->hasPosition) {
      this->_lastPosition = plane.mirror          (this->_lastPosition);
      this->_position     = plane.mirror          (this->_position);
      this->_direction    = plane.mirrorDirection (this->_direction);
    }
  }

  AffectedFaces getAffectedFaces () const {
    assert (this->mesh);
    assert (this->_parameters);

    AffectedFaces faces;
    this->mesh->intersects (this->sphere (), faces);

    if (this->_parameters->discardBack ()) {
      faces.filter ([this] (const WingedFace& f) {
        return glm::dot (this->direction (), f.triangle (*this->mesh).cross ()) <= 0.0f;
      });
    }
    return faces;
  }

  void sculpt (const VertexPtrSet& vertices) const {
    assert (this->_parameters);
    this->_parameters->sculpt (*this->self, vertices);
  }

  SBParameters* parametersPointer () const {
    return this->_parameters.get ();
  }

  void parametersPointer (SBParameters* p) {
    this->_parameters.reset (p);
  }
};

DELEGATE_BIG4MOVE_SELF (SculptBrush)
  
GETTER_CONST    (float              , SculptBrush, radius)
GETTER_CONST    (float              , SculptBrush, detailFactor)
GETTER_CONST    (float              , SculptBrush, stepWidthFactor)
GETTER_CONST    (bool               , SculptBrush, subdivide)
GETTER_CONST    (WingedMesh*        , SculptBrush, mesh)
SETTER          (float              , SculptBrush, radius)
SETTER          (float              , SculptBrush, detailFactor)
SETTER          (float              , SculptBrush, stepWidthFactor)
SETTER          (bool               , SculptBrush, subdivide)
SETTER          (WingedMesh*        , SculptBrush, mesh)
DELEGATE_CONST  (float              , SculptBrush, subdivThreshold)
GETTER_CONST    (bool               , SculptBrush, hasPosition)
DELEGATE_CONST  (const glm::vec3&   , SculptBrush, lastPosition)
DELEGATE_CONST  (const glm::vec3&   , SculptBrush, position)
DELEGATE_CONST  (const glm::vec3&   , SculptBrush, direction)
DELEGATE_CONST  (glm::vec3          , SculptBrush, delta)
DELEGATE_CONST  (PrimSphere         , SculptBrush, sphere)
DELEGATE2       (void               , SculptBrush, setPointOfAction, const glm::vec3&, const glm::vec3&)
DELEGATE2       (bool               , SculptBrush, updatePointOfAction, const glm::vec3&, const glm::vec3&)
DELEGATE        (void               , SculptBrush, resetPointOfAction)
DELEGATE1       (void               , SculptBrush, mirror, const PrimPlane&)
DELEGATE_CONST  (AffectedFaces      , SculptBrush, getAffectedFaces)
DELEGATE1_CONST (void               , SculptBrush, sculpt, const VertexPtrSet&)
DELEGATE_CONST  (SBParameters*      , SculptBrush, parametersPointer)
DELEGATE1       (void               , SculptBrush, parametersPointer, SBParameters*)
