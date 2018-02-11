/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
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

SBFlattenParameters::SBFlattenParameters ()
  : _lockPlane (false)
{
}

bool SBFlattenParameters::hasLockedPlane () const { return bool(this->_lockedPlane); }

const PrimPlane& SBFlattenParameters::lockedPlane () const { return *this->_lockedPlane; }

void SBFlattenParameters::lockedPlane (const PrimPlane& p) { this->_lockedPlane = p; }

void SBFlattenParameters::resetLockedPlane () { this->_lockedPlane.reset (); }

void SBFlattenParameters::mirror (const PrimPlane& m)
{
  if (this->hasLockedPlane ())
  {
    this->_lockedPlane->point (m.mirror (this->_lockedPlane->point ()));
    this->_lockedPlane->normal (m.mirrorDirection (this->_lockedPlane->normal ()));
  }
}

void SBDrawParameters::sculpt (const SculptBrush& brush, const DynamicFaces& faces) const
{
  if (faces.isEmpty () == false)
  {
    if (this->flat ())
    {
      const float     intensity = 0.3f * this->intensity ();
      const glm::vec3 planeNormal = this->invert (brush.normal ());
      const glm::vec3 planePos = brush.position () + (planeNormal * intensity * brush.radius ());
      const PrimPlane plane (planePos, planeNormal);

      brush.mesh ().forEachVertex (faces, [&brush, &plane, intensity](unsigned int i) {
        const glm::vec3& oldPos = brush.mesh ().vertex (i);
        const float      factor = intensity * Util::linearStep (oldPos, brush.position (),
                                                           0.5f * brush.radius (), brush.radius ());
        const float      distance = glm::min (0.0f, plane.distance (oldPos));
        const glm::vec3  newPos = oldPos - (plane.normal () * factor * distance);

        brush.mesh ().vertex (i, newPos);
      });
    }
    else
    {
      const float     intensity = 0.1f * this->intensity () * brush.radius ();
      const glm::vec3 avgDir = this->invert (brush.mesh ().averageNormal (faces));

      brush.mesh ().forEachVertex (faces, [&brush, &avgDir, intensity](unsigned int i) {
        const glm::vec3& oldPos = brush.mesh ().vertex (i);
        const float      factor =
          intensity * Util::smoothStep (oldPos, brush.position (), 0.0f, brush.radius ());
        const glm::vec3 newPos = oldPos + (factor * avgDir);

        brush.mesh ().vertex (i, newPos);
      });
    }
  }
}

void SBGrablikeParameters::sculpt (const SculptBrush& brush, const DynamicFaces& faces) const
{
  brush.mesh ().forEachVertex (faces, [&brush](unsigned int i) {
    const glm::vec3& oldPos = brush.mesh ().vertex (i);
    const float factor = Util::linearStep (oldPos, brush.lastPosition (), 0.0f, brush.radius ());
    const glm::vec3 newPos = oldPos + (factor * brush.delta ());

    brush.mesh ().vertex (i, newPos);
  });
}

void SBSmoothParameters::sculpt (const SculptBrush& brush, const DynamicFaces& faces) const
{
  brush.mesh ().forEachVertex (faces, [this, &brush](unsigned int i) {
    const glm::vec3  avgPos = brush.mesh ().averagePosition (i);
    const glm::vec3& oldPos = brush.mesh ().vertex (i);
    const glm::vec3  newPos = oldPos + (this->intensity () * (avgPos - oldPos));

    brush.mesh ().vertex (i, newPos);
  });
}

void SBReduceParameters::sculpt (const SculptBrush&, const DynamicFaces&) const {}

void SBFlattenParameters::sculpt (const SculptBrush& brush, const DynamicFaces& faces) const
{
  if (faces.isEmpty () == false)
  {
    PrimPlane plane (glm::vec3 (0.0f), glm::vec3 (1.0f));

    if (this->hasLockedPlane ())
    {
      plane = this->lockedPlane ();
    }
    else
    {
      glm::vec3 avgPos, avgNormal;
      brush.mesh ().average (faces, avgPos, avgNormal);
      plane = PrimPlane (avgPos, avgNormal);
    }

    brush.mesh ().forEachVertex (faces, [this, &brush, &plane](unsigned int i) {
      const glm::vec3& oldPos = brush.mesh ().vertex (i);
      const float      distance = plane.distance (oldPos);

      float factor =
        this->intensity () * Util::linearStep (oldPos, brush.position (), 0.0f, brush.radius ());
      factor *= this->hasLockedPlane () ? distance : glm::max (0.0f, distance);

      const glm::vec3 newPos = oldPos - (plane.normal () * factor);
      brush.mesh ().vertex (i, newPos);
    });
  }
}

void SBCreaseParameters::sculpt (const SculptBrush& brush, const DynamicFaces& faces) const
{
  if (faces.isEmpty () == false && brush.position () != brush.lastPosition ())
  {
    const glm::vec3 normal = this->invert (brush.normal ());

    brush.mesh ().forEachVertex (faces, [this, &brush, &normal](unsigned int i) {
      const glm::vec3& oldPos = brush.mesh ().vertex (i);
      const glm::vec3  delta = brush.position () - oldPos;
      const float      distance = glm::length (delta) / brush.radius ();

      if (distance <= 1.0f)
      {
        const float invDistance2 = (distance - 1.0f) * (distance - 1.0f);
        const float hFactor = invDistance2 * this->intensity ();
        float vFactor = invDistance2 * invDistance2 * brush.radius () * this->intensity () * 0.5f;
        const glm::vec3 newPos = (oldPos + (hFactor * delta)) + (normal * vFactor);

        brush.mesh ().vertex (i, newPos);
      }
    });
  }
}

void SBPinchParameters::sculpt (const SculptBrush& brush, const DynamicFaces& faces) const
{
  brush.mesh ().forEachVertex (faces, [&brush](unsigned int i) {
    const glm::vec3& oldPos = brush.mesh ().vertex (i);
    const glm::vec3  delta = brush.position () - oldPos;
    const float      distance = glm::length (delta) / brush.radius ();

    if (distance <= 1.0f)
    {
      const float     invDistance2 = (distance - 1.0f) * (distance - 1.0f);
      const float     hFactor = invDistance2 * 0.5f;
      const glm::vec3 newPos = oldPos + (hFactor * delta);

      brush.mesh ().vertex (i, newPos);
    }
  });
}

struct SculptBrush::Impl
{
  SculptBrush* self;
  float        radius;
  float        detailFactor;
  float        stepWidthFactor;
  bool         subdivide;
  DynamicMesh* _mesh;
  bool         hasPointOfAction;
  glm::vec3    _prevPosition;
  glm::vec3    _position;
  glm::vec3    _normal;

  std::unique_ptr<SBParameters> _parameters;

  Impl (SculptBrush* s)
    : self (s)
    , radius (0.0f)
    , detailFactor (0.0f)
    , stepWidthFactor (0.0f)
    , subdivide (true)
    , _mesh (nullptr)
    , hasPointOfAction (false)
  {
  }

  DynamicMesh& mesh () const
  {
    assert (this->hasPointOfAction);
    assert (this->_mesh);
    return *this->_mesh;
  }

  void mesh (DynamicMesh& m) { this->_mesh = &m; }

  float subdivThreshold () const
  {
    assert (this->detailFactor > 0.0f);
    assert (this->detailFactor < 1.0f);

    return (1.0f - this->detailFactor) * this->radius;
  }

  const glm::vec3& lastPosition () const
  {
    assert (this->hasPointOfAction);
    return this->_prevPosition;
  }

  const glm::vec3& position () const
  {
    assert (this->hasPointOfAction);
    return this->_position;
  }

  const glm::vec3& normal () const
  {
    assert (this->hasPointOfAction);
    return this->_normal;
  }

  glm::vec3 delta () const
  {
    assert (this->hasPointOfAction);
    return this->_position - this->_prevPosition;
  }

  PrimSphere sphere () const
  {
    assert (this->_parameters);

    const glm::vec3& pos =
      this->_parameters->useLastPos () ? this->lastPosition () : this->position ();
    return PrimSphere (pos, this->radius);
  }

  float stepWidth () const { return this->stepWidthFactor * glm::log (this->self->radius () + 1); }

  void setPointOfAction (DynamicMesh& mesh, const glm::vec3& p, const glm::vec3& n)
  {
    this->_mesh = &mesh;
    this->_prevPosition = this->hasPointOfAction ? this->_position : p;
    this->_position = p;
    this->_normal = n;
    this->hasPointOfAction = true;
  }

  void resetPointOfAction ()
  {
    this->hasPointOfAction = false;
    this->_mesh = nullptr;
  }

  void mirror (const PrimPlane& plane)
  {
    if (this->_parameters)
    {
      this->_parameters->mirror (plane);
    }

    if (this->hasPointOfAction)
    {
      this->_prevPosition = plane.mirror (this->_prevPosition);
      this->_position = plane.mirror (this->_position);
      this->_normal = plane.mirrorDirection (this->_normal);
    }
  }

  DynamicFaces getAffectedFaces () const
  {
    assert (this->hasPointOfAction);
    assert (this->_parameters);

    DynamicFaces faces;
    this->_mesh->intersects (this->sphere (), faces);

    if (this->_parameters->discardBack ())
    {
      faces.filter ([this](unsigned int i) {
        return glm::dot (this->normal (), this->_mesh->face (i).cross ()) > 0.0f;
      });
    }
    return faces;
  }

  void sculpt (const DynamicFaces& faces) const
  {
    assert (this->_parameters);
    this->_parameters->sculpt (*this->self, faces);
  }

  SBParameters* parametersPointer () const { return this->_parameters.get (); }

  void parametersPointer (SBParameters* p) { this->_parameters.reset (p); }
};

DELEGATE_BIG3_SELF (SculptBrush)

GETTER_CONST (float, SculptBrush, radius)
GETTER_CONST (float, SculptBrush, detailFactor)
GETTER_CONST (float, SculptBrush, stepWidthFactor)
GETTER_CONST (bool, SculptBrush, subdivide)
DELEGATE_CONST (DynamicMesh&, SculptBrush, mesh)
SETTER (float, SculptBrush, radius)
SETTER (float, SculptBrush, detailFactor)
SETTER (float, SculptBrush, stepWidthFactor)
SETTER (bool, SculptBrush, subdivide)
DELEGATE_CONST (float, SculptBrush, subdivThreshold)
DELEGATE_CONST (const glm::vec3&, SculptBrush, lastPosition)
DELEGATE_CONST (const glm::vec3&, SculptBrush, position)
DELEGATE_CONST (const glm::vec3&, SculptBrush, normal)
DELEGATE_CONST (glm::vec3, SculptBrush, delta)
DELEGATE_CONST (PrimSphere, SculptBrush, sphere)
DELEGATE_CONST (float, SculptBrush, stepWidth)
GETTER_CONST (bool, SculptBrush, hasPointOfAction)
DELEGATE3 (void, SculptBrush, setPointOfAction, DynamicMesh&, const glm::vec3&, const glm::vec3&)
DELEGATE (void, SculptBrush, resetPointOfAction)
DELEGATE1 (void, SculptBrush, mirror, const PrimPlane&)
DELEGATE_CONST (DynamicFaces, SculptBrush, getAffectedFaces)
DELEGATE1_CONST (void, SculptBrush, sculpt, const DynamicFaces&)
DELEGATE_CONST (SBParameters*, SculptBrush, parametersPointer)
DELEGATE1 (void, SculptBrush, parametersPointer, SBParameters*)
