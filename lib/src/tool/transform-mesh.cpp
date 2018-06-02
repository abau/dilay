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

  QButtonGroup* modeEdit;
  QWidget*      moveEdit;
  QButtonGroup* rotationOriginEdit;

  Impl (ToolTransformMesh* s)
    : self (s)
    , mesh (nullptr)
    , mode (Mode (s->cache ().get<int> ("mode", int(Mode::Move))))
    , movement (s->state ().camera (), false)
    , rotation (s->state ().camera ())
    , rotationOrigin (
        RotationOrigin (s->cache ().get<int> ("rotation-rigin", int(RotationOrigin::Center))))
    , modeEdit (nullptr)
    , moveEdit (nullptr)
    , rotationOriginEdit (nullptr)
  {
  }

  ToolResponse runInitialize ()
  {
    this->setupProperties ();
    this->setupToolTip ();

    return ToolResponse::None;
  }

  void setMode (Mode m)
  {
    assert (this->modeEdit != nullptr);
    assert (this->moveEdit != nullptr);
    assert (this->rotationOriginEdit != nullptr);

    this->mode = m;

    this->modeEdit->button (int(Mode::Move))->setChecked (m == Mode::Move);
    this->modeEdit->button (int(Mode::Rotate))->setChecked (m == Mode::Rotate);

    this->moveEdit->setVisible (m == Mode::Move);
    for (QAbstractButton* button : this->rotationOriginEdit->buttons ())
    {
      button->setVisible (m == Mode::Rotate);
    }
    this->setupToolTip ();
  };

  void setupProperties ()
  {
    assert (this->modeEdit == nullptr);
    assert (this->moveEdit == nullptr);
    assert (this->rotationOriginEdit == nullptr);

    ViewTwoColumnGrid& properties = this->self->properties ();

    this->modeEdit = &ViewUtil::buttonGroup ({QObject::tr ("Move"), QObject::tr ("Rotate")});
    properties.add (*modeEdit);
    properties.addSeparator ();

    this->moveEdit = &this->self->addMoveOnPrimaryPlaneProperties (this->movement);

    this->rotationOriginEdit = &ViewUtil::buttonGroup (
      {QObject::tr ("Intersection"), QObject::tr ("Center"), QObject::tr ("Origin")});
    ViewUtil::connect (*this->rotationOriginEdit, int(this->rotationOrigin), [this](int id) {
      this->rotationOrigin = RotationOrigin (id);
      this->self->cache ().set ("rotation-rigin", id);
    });
    properties.add (*this->rotationOriginEdit);

    ViewUtil::connect (*this->modeEdit, int(this->mode), [this](int id) {
      this->self->cache ().set ("mode", id);
      this->setMode (Mode (id));
    });
  }

  void setupToolTip ()
  {
    ViewToolTip toolTip;
    switch (this->mode)
    {
      case Mode::Move:
        toolTip.add (ViewInputEvent::MouseLeft, QObject::tr ("Drag to move"));
        break;

      case Mode::Rotate:
        toolTip.add (ViewInputEvent::MouseLeft, QObject::tr ("Drag to rotate"));
        break;
    }
    this->self->setToolTip (toolTip);
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
        if (this->mode == Mode::Move)
        {
          this->movement.reset (intersection.position ());
        }
        else if (this->mode == Mode::Rotate)
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
