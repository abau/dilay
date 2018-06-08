/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QAbstractButton>
#include <QButtonGroup>
#include <glm/glm.hpp>
#include "cache.hpp"
#include "dynamic/mesh-intersection.hpp"
#include "dynamic/mesh.hpp"
#include "mesh.hpp"
#include "primitive/aabox.hpp"
#include "state.hpp"
#include "tool/util/movement.hpp"
#include "tool/util/rotation.hpp"
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
    Rotate
  };

  enum class RotationOrigin
  {
    Intersection,
    Center,
    Origin
  };
}

struct ToolTransformMesh::Impl
{
  ToolTransformMesh* self;
  DynamicMesh*       mesh;
  Mode               mode;
  ToolUtilMovement   movement;
  ToolUtilRotation   rotation;
  RotationOrigin     rotationOrigin;

  Impl (ToolTransformMesh* s)
    : self (s)
    , mesh (nullptr)
    , movement (s->state ().camera (), false)
    , rotation (s->state ().camera ())
    , rotationOrigin (
        RotationOrigin (s->cache ().get<int> ("rotation-rigin", int(RotationOrigin::Center))))
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

    QButtonGroup& rotationOriginEdit = ViewUtil::buttonGroup (
      {QObject::tr ("Intersection"), QObject::tr ("Center"), QObject::tr ("Origin")});
    ViewUtil::connect (rotationOriginEdit, int(rotationOrigin), [this](int id) {
      this->rotationOrigin = RotationOrigin (id);
      this->self->cache ().set ("rotation-rigin", id);
    });
    properties.addStacked (QObject::tr ("Rotation"), rotationOriginEdit);
  }

  void setupToolTip ()
  {
    ViewToolTip toolTip;
    toolTip.add (ViewInputEvent::MouseLeft, QObject::tr ("Drag to move"));
    toolTip.add (ViewInputEvent::MouseLeft, ViewInputModifier::Shift,
                 QObject::tr ("Drag to rotate"));

    this->self->state ().setToolTip (&toolTip);
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
          this->movement.reset (intersection.position ());
          this->mode = Mode::Move;
        }
        else if (e.modifiers () == Qt::ShiftModifier)
        {
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
        this->self->snapshotDynamicMeshes ();
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
