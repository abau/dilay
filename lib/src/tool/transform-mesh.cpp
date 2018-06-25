/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QAbstractButton>
#include <QButtonGroup>
#include <glm/glm.hpp>
#include "cache.hpp"
#include "camera.hpp"
#include "dimension.hpp"
#include "dynamic/mesh-intersection.hpp"
#include "dynamic/mesh.hpp"
#include "mesh.hpp"
#include "primitive/aabox.hpp"
#include "state.hpp"
#include "tool/util/movement.hpp"
#include "tool/util/rotation.hpp"
#include "tool/util/scaling.hpp"
#include "tools.hpp"
#include "view/pointing-event.hpp"
#include "view/shortcut.hpp"
#include "view/tool-tip.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

namespace
{
  enum class Mode
  {
    Move,
    Rotate,
    Scale
  };

  enum class RotationOrigin
  {
    Intersection,
    Center,
    Origin
  };

  enum class ScalingMode
  {
    Uniform,
    PrimaryPlane
  };
}

struct ToolTransformMesh::Impl
{
  ToolTransformMesh* self;
  DynamicMesh*       mesh;
  Mode               mode;
  ToolUtilMovement   movement;
  ToolUtilScaling    scaling;
  ScalingMode        scalingMode;
  glm::vec3          scalingSize;
  ToolUtilRotation   rotation;
  RotationOrigin     rotationOrigin;

  Impl (ToolTransformMesh* s)
    : self (s)
    , mesh (nullptr)
    , movement (s->state ().camera (), false)
    , scaling (s->state ().camera ())
    , scalingMode (ScalingMode (s->cache ().get<int> ("scaling-mode", int(ScalingMode::Uniform))))
    , rotation (s->state ().camera ())
    , rotationOrigin (
        RotationOrigin (s->cache ().get<int> ("rotation-origin", int(RotationOrigin::Center))))
  {
  }

  ToolResponse runInitialize ()
  {
    this->setupProperties ();
    this->setupToolTip ();

    return ToolResponse::None;
  }

  void setupProperties ()
  {
    ViewTwoColumnGrid& properties = this->self->properties ();

    this->self->addMoveOnPrimaryPlaneProperties (this->movement);

    properties.addSeparator ();

    QButtonGroup& scalingModeEdit = ViewUtil::buttonGroup (
      {QObject::tr ("Scale uniformly"), QObject::tr ("Scale on primary plane")});
    ViewUtil::connect (scalingModeEdit, int(this->scalingMode), [this](int id) {
      this->scalingMode = ScalingMode (id);
      this->self->cache ().set ("scaling-mode", id);
    });
    properties.add (scalingModeEdit);

    properties.addSeparator ();

    QButtonGroup& rotationOriginEdit =
      ViewUtil::buttonGroup ({QObject::tr ("Rotate at intersection"),
                              QObject::tr ("Rotate at center"), QObject::tr ("Rotate at origin")});
    ViewUtil::connect (rotationOriginEdit, int(this->rotationOrigin), [this](int id) {
      this->rotationOrigin = RotationOrigin (id);
      this->self->cache ().set ("rotation-origin", id);
    });
    properties.add (rotationOriginEdit);
  }

  void setupToolTip ()
  {
    ViewToolTip toolTip;
    toolTip.add (ViewInputEvent::MouseLeft, QObject::tr ("Drag to move"));
    toolTip.add (ViewInputEvent::MouseLeft, ViewInputModifier::Shift,
                 QObject::tr ("Drag to scale"));
    toolTip.add (ViewInputEvent::MouseLeft, ViewInputModifier::Ctrl,
                 QObject::tr ("Drag to rotate"));

    this->self->state ().setToolTip (&toolTip);
  }

  glm::vec3 scalingVector () const
  {
    assert (this->mesh);

    const float     minExtent = 0.01f;
    const glm::vec3 extent = this->scalingSize * this->mesh->scaling ();
    glm::vec3       scaling = glm::vec3 (1.0f);

    switch (this->scalingMode)
    {
      case ScalingMode::Uniform:
        scaling = glm::vec3 (this->scaling.factor ());
        break;

      case ScalingMode::PrimaryPlane:
        switch (this->self->state ().camera ().primaryDimension ())
        {
          case Dimension::X:
            scaling.z = this->scaling.factorRight ();
            scaling.y = this->scaling.factorUp ();
            break;
          case Dimension::Y:
            scaling.x = this->scaling.factorRight ();
            scaling.z = this->scaling.factorUp ();
            break;
          case Dimension::Z:
            scaling.x = this->scaling.factorRight ();
            scaling.y = this->scaling.factorUp ();
            break;
        }
    }

    if (scaling.x * extent.x < minExtent)
    {
      scaling.x = 1.0f;
    }
    if (scaling.y * extent.y < minExtent)
    {
      scaling.y = 1.0f;
    }
    if (scaling.z * extent.z < minExtent)
    {
      scaling.z = 1.0f;
    }
    return scaling;
  }

  ToolResponse runMoveEvent (const ViewPointingEvent& e)
  {
    if (e.leftButton () && this->mesh)
    {
      if (this->mode == Mode::Move && this->movement.move (e))
      {
        this->mesh->translate (this->movement.delta ());
        return ToolResponse::Redraw;
      }
      else if (this->mode == Mode::Scale && this->scaling.move (e))
      {
        this->mesh->scale (this->scalingVector ());
        return ToolResponse::Redraw;
      }
      else if (this->mode == Mode::Rotate && this->rotation.rotate (e))
      {
        this->mesh->rotate (this->rotation.matrix ());
        return ToolResponse::Redraw;
      }
    }
    return ToolResponse::None;
  }

  ToolResponse runPressEvent (const ViewPointingEvent& e)
  {
    if (e.leftButton ())
    {
      DynamicMeshIntersection intersection;
      if (this->self->intersectsScene (e, intersection))
      {
        this->mesh = &intersection.mesh ();
        if (e.modifiers () == Qt::NoModifier)
        {
          this->self->snapshotDynamicMeshes ();

          this->movement.reset (intersection.position ());
          this->mode = Mode::Move;
        }
        else if (e.modifiers () == Qt::ShiftModifier)
        {
          this->self->snapshotDynamicMeshes ();

          const PrimAABox bounds = this->mesh->mesh ().bounds ();
          this->scalingSize = bounds.maximum () - bounds.minimum ();
          this->scaling.reset (bounds.center (), intersection.position ());

          this->mesh->position (-bounds.center ());
          this->mesh->normalize ();
          this->mesh->bufferData ();
          this->mesh->position (bounds.center ());

          this->mode = Mode::Scale;
        }
        else if (e.modifiers () == Qt::ControlModifier)
        {
          this->self->snapshotDynamicMeshes ();

          switch (this->rotationOrigin)
          {
            case RotationOrigin::Intersection:
              this->rotation.reset (intersection.position ());
              break;
            case RotationOrigin::Center:
              this->rotation.reset (this->mesh->mesh ().bounds ().center ());
              break;
            case RotationOrigin::Origin:
              this->rotation.reset (glm::vec3 (0.0f));
              break;
          }
          this->mode = Mode::Rotate;
        }
      }
    }
    return ToolResponse::None;
  }

  ToolResponse runReleaseEvent (const ViewPointingEvent& e)
  {
    if (e.leftButton ())
    {
      return this->runCommit ();
    }
    return ToolResponse::None;
  }

  ToolResponse runCommit ()
  {
    if (this->mesh)
    {
      this->mesh->normalize ();
      this->mesh->bufferData ();
      this->mesh = nullptr;
    }
    return ToolResponse::None;
  }
};

DELEGATE_TOOL (ToolTransformMesh)
DELEGATE_TOOL_RUN_MOVE_EVENT (ToolTransformMesh)
DELEGATE_TOOL_RUN_PRESS_EVENT (ToolTransformMesh)
DELEGATE_TOOL_RUN_RELEASE_EVENT (ToolTransformMesh)
DELEGATE_TOOL_RUN_COMMIT (ToolTransformMesh)
