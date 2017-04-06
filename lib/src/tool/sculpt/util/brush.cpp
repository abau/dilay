/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <memory>
#include "dynamic/faces.hpp"
#include "dynamic/mesh.hpp"
#include "primitive/plane.hpp"
#include "primitive/sphere.hpp"
#include "primitive/triangle.hpp"
#include "tool/sculpt/util/brush.hpp"
#include "util.hpp"

void SBDrawParameters :: sculpt (const SculptBrush& brush, const DynamicFaces& faces) const {
  if (faces.isEmpty () == false) {
    if (this->flat ()) {
      const float intensity = 0.3f * this->intensity ();
      const glm::vec3 avgNormal = brush.mesh ().averageNormal (faces);
      const glm::vec3 planePos = brush.position () + (avgNormal * intensity * brush.radius ());
      const PrimPlane plane (planePos, avgNormal);

      brush.mesh ().forEachVertex (faces, [this, &brush, &avgNormal, &plane, intensity]
                                          (unsigned int i)
      {
        const glm::vec3& oldPos   = brush.mesh ().vertex (i);
        const float      factor   = intensity
                                  * Util::linearStep ( oldPos, brush.position ()
                                                     , 0.5f * brush.radius (), brush.radius () );
        const float      distance = glm::min (0.0f, plane.distance (oldPos));
        const glm::vec3  newPos   = oldPos - (avgNormal * factor * distance);

        brush.mesh ().vertex (i, newPos);
      });
    }
    else {
      const float intensity = 0.1f * this->intensity () * brush.radius ();
      const glm::vec3 avgDir = this->invert (brush.mesh ().averageNormal (faces));

      brush.mesh ().forEachVertex (faces, [this, &brush, &avgDir, intensity] (unsigned int i) {
        const glm::vec3& oldPos    = brush.mesh ().vertex (i);
        const float      factor    = intensity
                                   * Util::smoothStep ( oldPos, brush.position ()
                                                      , 0.0f, brush.radius () );
        const glm::vec3 newPos = oldPos + (factor * avgDir);

        brush.mesh ().vertex (i, newPos);
      });
    }
  }
}

void SBDraglikeParameters :: sculpt (const SculptBrush& brush, const DynamicFaces& faces) const {
  float (*stepFunction) (const glm::vec3&, const glm::vec3&, float, float) =
    this->linearStep () ? Util::linearStep : Util::smoothStep;

  brush.mesh ().forEachVertex (faces, [this, &brush, stepFunction] (unsigned int i) {
    const glm::vec3& oldPos    = brush.mesh ().vertex (i);
    const float      innerRadius = (1.0f - this->smoothness ()) * brush.radius ();
    const float      factor      = stepFunction ( oldPos, brush.lastPosition ()
                                                , innerRadius, brush.radius () );
    const glm::vec3  newPos      = oldPos + (factor * brush.delta ());

    brush.mesh ().vertex (i, newPos);
  });
}

void SBSmoothParameters :: sculpt (const SculptBrush& brush, const DynamicFaces& faces) const {
  if (this->relaxOnly () == false) {
    brush.mesh ().forEachVertex (faces, [this, &brush] (unsigned int i) {
      const glm::vec3 avgPos = brush.mesh ().averagePosition (i);

      const glm::vec3& oldPos = brush.mesh ().vertex (i);
      const float      factor = this->intensity ()
                              * Util::smoothStep ( oldPos, brush.position ()
                                                 , 0.0f, brush.radius () );
      const glm::vec3  newPos = oldPos + (factor * (avgPos - oldPos));

      brush.mesh ().vertex (i, newPos);
    });
  }
}

void SBReduceParameters :: sculpt (const SculptBrush&, const DynamicFaces&) const {}

void SBFlattenParameters :: sculpt (const SculptBrush& brush, const DynamicFaces& faces) const {
  if (faces.isEmpty () == false) {
    glm::vec3 avgPos, avgNormal;
    brush.mesh ().average (faces, avgPos, avgNormal);

    const PrimPlane plane (avgPos, avgNormal);

    brush.mesh ().forEachVertex (faces, [this, &brush, &avgNormal, &plane] (unsigned int i) {
      const glm::vec3& oldPos   = brush.mesh ().vertex (i);
      const float      factor   = this->intensity ()
                                * Util::linearStep ( oldPos, brush.position ()
                                                   , 0.0f, brush.radius () );
      const float      distance = glm::max (0.0f, plane.distance (oldPos));
      const glm::vec3  newPos   = oldPos - (avgNormal * factor * distance);

      brush.mesh ().vertex (i, newPos);
    });
  }
}

void SBCreaseParameters :: sculpt (const SculptBrush& brush, const DynamicFaces& faces) const {
  if (faces.isEmpty () == false) {
    const glm::vec3 avgDir = this->invert (brush.mesh ().averageNormal (faces));
    const glm::vec3 refPos = brush.position () + (avgDir * this->intensity () * brush.radius ());

    const PrimPlane plane (refPos, avgDir);

    brush.mesh ().forEachVertex (faces, [this, &brush, &avgDir, &refPos, &plane] (unsigned int i) {
      const glm::vec3& oldPos   = brush.mesh ().vertex (i);
      const glm::vec3  projPos  = plane.project (oldPos);
      const float      distance = glm::distance (projPos, refPos);

      if (glm::distance (projPos, refPos) > 0.001f) {
        const float     relDistance = glm::clamp (distance / brush.radius (), 0.0f, 1.0f);
        const float     factor      = 0.1f * brush.radius () * glm::min (0.5f, 1.0f - relDistance);
        const glm::vec3 direction   = glm::normalize ( (projPos - oldPos)
                                                     + (2.0f * (refPos - projPos)) );
        const glm::vec3 newPos      = oldPos + (factor * direction);

        brush.mesh ().vertex (i, newPos);
      }
    });
  }
}

void SBPinchParameters :: sculpt (const SculptBrush& brush, const DynamicFaces& faces) const {
  brush.mesh ().forEachVertex (faces, [this, &brush] (unsigned int i) {
    const glm::vec3& oldPos   = brush.mesh ().vertex (i);
    const float      distance = glm::distance (oldPos, brush.position ());

    if (distance > 0.001f) {
      const float     relDistance = glm::clamp (distance / brush.radius (), 0.0f, 1.0f);
      const float     factor      = 0.1f * brush.radius () * glm::min (0.5f, 1.0f - relDistance);
      const glm::vec3 direction   = this->invert (glm::normalize (brush.position () - oldPos));
      const glm::vec3 newPos      = oldPos + (factor * direction);

      brush.mesh ().vertex (i, newPos);
    }
  });
}

struct SculptBrush :: Impl {
  SculptBrush*  self;
  float         radius;
  float         detailFactor;
  float         stepWidthFactor;
  bool          subdivide;
  DynamicMesh* _mesh;
  bool          hasPointOfAction;
  glm::vec3    _lastPosition;
  glm::vec3    _position;
  glm::vec3    _normal;

  std::unique_ptr <SBParameters> _parameters;

  Impl (SculptBrush* s) 
    : self             (s)
    , radius           (0.0f)
    , detailFactor     (0.0f)
    , stepWidthFactor  (0.0f)
    , subdivide        (false)
    ,_mesh             (nullptr)
    , hasPointOfAction (false)
  {}

  DynamicMesh& mesh () const {
    assert (this->hasPointOfAction);
    assert (this->_mesh);
    return *this->_mesh;
  }

  void mesh (DynamicMesh& m) {
    this->_mesh = &m;
  }

  float subdivThreshold () const {
    return (1.0f - this->detailFactor) * this->radius;
  }

  const glm::vec3& lastPosition () const {
    assert (this->hasPointOfAction);
    return this->_lastPosition;
  }

  const glm::vec3& position () const {
    assert (this->hasPointOfAction);
    return this->_position;
  }

  const glm::vec3& normal () const {
    assert (this->hasPointOfAction);
    return this->_normal;
  }

  glm::vec3 delta () const {
    assert (this->hasPointOfAction);
    return this->_position - this->_lastPosition;
  }

  PrimSphere sphere () const {
    const glm::vec3& pos = this->_parameters->useLastPos () ? this->lastPosition ()
                                                            : this->position ();
    return PrimSphere (pos, this->radius);
  }

  float stepWidth () const {
    return this->stepWidthFactor * glm::log (this->self->radius () + 1);
  }

  void setPointOfAction (DynamicMesh& mesh, const glm::vec3& p, const glm::vec3& n) {
    this->_mesh = &mesh;
    this->_lastPosition = this->hasPointOfAction ? this->_position : p;
    this->_position = p;
    this->_normal = n;
    this->hasPointOfAction = true;
  }

  void resetPointOfAction () {
    this->hasPointOfAction = false;
    this->_mesh = nullptr;
  }

  void mirror (const PrimPlane& plane) {
    if (this->hasPointOfAction) {
      this->_lastPosition = plane.mirror          (this->_lastPosition);
      this->_position     = plane.mirror          (this->_position);
      this->_normal       = plane.mirrorDirection (this->_normal);
    }
  }

  DynamicFaces getAffectedFaces () const {
    assert (this->hasPointOfAction);
    assert (this->_parameters);

    DynamicFaces faces;
    this->_mesh->intersects (this->sphere (), faces);

    if (this->_parameters->discardBack ()) {
      faces.filter ([this] (unsigned int i) {
        return glm::dot (this->normal (), this->_mesh->face (i).cross ()) > 0.0f;
      });
    }
    return faces;
  }

  void sculpt (const DynamicFaces& faces) const {
    assert (this->_parameters);
    this->_parameters->sculpt (*this->self, faces);
  }

  SBParameters* parametersPointer () const {
    return this->_parameters.get ();
  }

  void parametersPointer (SBParameters* p) {
    this->_parameters.reset (p);
  }
};

DELEGATE_BIG3_SELF (SculptBrush)
  
GETTER_CONST    (float              , SculptBrush, radius)
GETTER_CONST    (float              , SculptBrush, detailFactor)
GETTER_CONST    (float              , SculptBrush, stepWidthFactor)
GETTER_CONST    (bool               , SculptBrush, subdivide)
DELEGATE_CONST  (DynamicMesh&       , SculptBrush, mesh)
SETTER          (float              , SculptBrush, radius)
SETTER          (float              , SculptBrush, detailFactor)
SETTER          (float              , SculptBrush, stepWidthFactor)
SETTER          (bool               , SculptBrush, subdivide)
DELEGATE_CONST  (float              , SculptBrush, subdivThreshold)
DELEGATE_CONST  (const glm::vec3&   , SculptBrush, lastPosition)
DELEGATE_CONST  (const glm::vec3&   , SculptBrush, position)
DELEGATE_CONST  (const glm::vec3&   , SculptBrush, normal)
DELEGATE_CONST  (glm::vec3          , SculptBrush, delta)
DELEGATE_CONST  (PrimSphere         , SculptBrush, sphere)
DELEGATE_CONST  (float              , SculptBrush, stepWidth)
GETTER_CONST    (bool               , SculptBrush, hasPointOfAction)
DELEGATE3       (void               , SculptBrush, setPointOfAction, DynamicMesh&, const glm::vec3&, const glm::vec3&)
DELEGATE        (void               , SculptBrush, resetPointOfAction)
DELEGATE1       (void               , SculptBrush, mirror, const PrimPlane&)
DELEGATE_CONST  (DynamicFaces       , SculptBrush, getAffectedFaces)
DELEGATE1_CONST (void               , SculptBrush, sculpt, const DynamicFaces&)
DELEGATE_CONST  (SBParameters*      , SculptBrush, parametersPointer)
DELEGATE1       (void               , SculptBrush, parametersPointer, SBParameters*)
