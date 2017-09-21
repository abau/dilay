/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QAbstractButton>
#include <QButtonGroup>
#include <QFrame>
#include <QPainter>
#include <QSlider>
#include <QWheelEvent>
#include "cache.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "config.hpp"
#include "dynamic/mesh-intersection.hpp"
#include "dynamic/mesh.hpp"
#include "history.hpp"
#include "mirror.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool/trim-mesh/action.hpp"
#include "tool/trim-mesh/border.hpp"
#include "tool/trim-mesh/split-mesh.hpp"
#include "tools.hpp"
#include "view/main-window.hpp"
#include "view/pointing-event.hpp"
#include "view/tool-tip.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

namespace
{
  enum class TrimStatus
  {
    Trimmed,
    NotTrimmed,
    Failed
  };

  enum class TrimMode
  {
    Normal,
    Slice,
    Cut
  };
}

struct ToolTrimMesh::Impl
{
  ToolTrimMesh*           self;
  std::vector<glm::ivec2> points;
  TrimMode                trimMode;
  QSlider&                widthEdit;

  Impl (ToolTrimMesh* s)
    : self (s)
    , trimMode (TrimMode (s->cache ().get<int> ("trim-mode", int(TrimMode::Normal))))
    , widthEdit (ViewUtil::slider (1, s->cache ().get<int> ("trim-width", 10), 200))
  {
  }

  void setupProperties ()
  {
    ViewTwoColumnGrid& properties = this->self->properties ();

    this->self->addMirrorProperties (true);
    properties.add (ViewUtil::horizontalLine ());

    this->widthEdit.setSingleStep (10);
    ViewUtil::connect (this->widthEdit,
                       [this](int w) { this->self->cache ().set ("trim-width", w); });

    QButtonGroup& trimModeEdit = *new QButtonGroup;
    properties.add (trimModeEdit,
                    {QObject::tr ("Normal"), QObject::tr ("Slice"), QObject::tr ("Cut")});
    ViewUtil::connect (trimModeEdit, [this](int id) {
      this->trimMode = TrimMode (id);
      this->self->cache ().set ("trim-mode", id);
      widthEdit.setEnabled (this->trimMode != TrimMode::Normal);
    });
    trimModeEdit.button (int(this->trimMode))->click ();

    properties.addStacked (QObject::tr ("Width"), this->widthEdit);
  }

  void setupToolTip ()
  {
    ViewToolTip toolTip;
    toolTip.add (ViewToolTip::Event::MouseLeft, QObject::tr ("Drag to trim"));
    toolTip.add (ViewToolTip::Event::MouseWheel, ViewToolTip::Modifier::Shift,
                 QObject::tr ("Change slice/cut width"));
    this->self->showToolTip (toolTip);
  }

  ToolResponse runInitialize ()
  {
    this->self->renderMirror (true);

    this->setupProperties ();
    this->setupToolTip ();

    return ToolResponse::None;
  }

  ToolResponse runMoveEvent (const ViewPointingEvent&)
  {
    return this->points.empty () ? ToolResponse::None : ToolResponse::Redraw;
  }

  ToolResponse runPressEvent (const ViewPointingEvent&) { return ToolResponse::None; }

  ToolResponse runWheelEvent (const QWheelEvent& e)
  {
    if (e.orientation () == Qt::Vertical)
    {
      if (e.modifiers () == Qt::ShiftModifier)
      {
        if (e.delta () > 0)
        {
          this->widthEdit.setValue (this->widthEdit.value () + this->widthEdit.singleStep ());
        }
        else if (e.delta () < 0)
        {
          this->widthEdit.setValue (this->widthEdit.value () - this->widthEdit.singleStep ());
        }
      }
    }
    return ToolResponse::Redraw;
  }

  TrimStatus trimMesh (ToolTrimMeshBorder& border)
  {
    if (ToolTrimMeshSplitMesh::splitMesh (border))
    {
      if (border.hasVertices ())
      {
        if (ToolTrimMeshAction::trimMesh (border))
        {
          return TrimStatus::Trimmed;
        }
        else
        {
          return TrimStatus::Failed;
        }
      }
      else
      {
        return TrimStatus::NotTrimmed;
      }
    }
    else
    {
      return TrimStatus::Failed;
    }
  }

  TrimStatus trimMesh (DynamicMesh& mesh, int offset, bool reverse)
  {
    assert (this->points.size () == 2);

    const glm::ivec2& p1 = reverse ? this->points[1] : this->points[0];
    const glm::ivec2& p2 = reverse ? this->points[0] : this->points[1];
    const float       fOffset = 0.5f * float(offset);
    const glm::ivec2  orth =
      glm::ceil (glm::normalize (glm::vec2 (Util::orthogonalRight (p2 - p1))) * fOffset);
    const PrimRay ray1 = this->self->state ().camera ().ray (p1 + orth);
    const PrimRay ray2 = this->self->state ().camera ().ray (p2 + orth);

    ToolTrimMeshBorder border (mesh, ray1, ray2);

    if (this->self->hasMirror () == false)
    {
      return this->trimMesh (border);
    }
    else
    {
      ToolTrimMeshBorder mBorder = border.mirror (this->self->mirror ().plane ());
      const TrimStatus   status = this->trimMesh (border);

      if (status == TrimStatus::Trimmed)
      {
        if (this->trimMesh (mBorder) == TrimStatus::Failed)
        {
          return TrimStatus::Failed;
        }
        else
        {
          return status;
        }
      }
      else
      {
        return status;
      }
    }
  }

  TrimStatus trimMesh (DynamicMesh& mesh)
  {
    if (this->trimMode == TrimMode::Normal)
    {
      return this->trimMesh (mesh, 0.0f, false);
    }
    else if (this->trimMode == TrimMode::Slice)
    {
      const TrimStatus status = this->trimMesh (mesh, this->widthEdit.value (), false);
      if (status != TrimStatus::Failed)
      {
        switch (this->trimMesh (mesh, this->widthEdit.value (), true))
        {
          case TrimStatus::Trimmed:
            return TrimStatus::Trimmed;

          case TrimStatus::NotTrimmed:
            return status;

          case TrimStatus::Failed:
            return TrimStatus::Failed;

          default:
            DILAY_IMPOSSIBLE
        }
      }
      else
      {
        return status;
      }
    }
    else if (this->trimMode == TrimMode::Cut)
    {
      DynamicMesh& mesh2 =
        this->self->state ().scene ().newDynamicMesh (this->self->config (), mesh);

      switch (this->trimMesh (mesh, -this->widthEdit.value (), false))
      {
        case TrimStatus::Failed:
          this->self->state ().scene ().deleteMesh (mesh2);
          return TrimStatus::Failed;

        case TrimStatus::NotTrimmed:
          this->self->state ().scene ().deleteMesh (mesh2);
          return this->trimMesh (mesh, -this->widthEdit.value (), true);

        case TrimStatus::Trimmed:
          switch (this->trimMesh (mesh2, -this->widthEdit.value (), true))
          {
            case TrimStatus::Trimmed:
              return TrimStatus::Trimmed;

            case TrimStatus::NotTrimmed:
              this->self->state ().scene ().deleteMesh (mesh2);
              return TrimStatus::Trimmed;

            case TrimStatus::Failed:
              this->self->state ().scene ().deleteMesh (mesh2);
              return TrimStatus::Failed;

            default:
              DILAY_IMPOSSIBLE
          }
        default:
          DILAY_IMPOSSIBLE
      }
    }
    else
    {
      DILAY_IMPOSSIBLE
    }
  }

  ToolResponse runReleaseEvent (const ViewPointingEvent& e)
  {
    if (e.leftButton () == false)
    {
      return ToolResponse::None;
    }
    else
    {
      DynamicMeshIntersection intersection;
      if (this->self->intersectsScene (e.position (), intersection) == false)
      {
        this->points.push_back (e.position ());
      }

      if (this->points.size () == 2)
      {
        this->self->snapshotDynamicMeshes ();

        TrimStatus status = TrimStatus::NotTrimmed;
        this->self->state ().scene ().forEachMesh ([&status, this](DynamicMesh& mesh) {
          if (status != TrimStatus::Failed)
          {
            const TrimStatus s = this->trimMesh (mesh);
            if (s != TrimStatus::NotTrimmed)
            {
              status = s;
            }
          }
        });

        switch (status)
        {
          case TrimStatus::Trimmed:
            this->self->state ().scene ().deleteEmptyMeshes ();
            break;

          case TrimStatus::NotTrimmed:
            this->self->state ().history ().dropPastSnapshot ();
            break;

          case TrimStatus::Failed:
            this->self->state ().undo ();
            this->self->state ().history ().dropFutureSnapshot ();
            ViewUtil::error (this->self->state ().mainWindow (),
                             QObject::tr ("Could not trim mesh."));
            break;
        }
        this->points.clear ();
      }
      return ToolResponse::Redraw;
    }
  }

  void runPaint (QPainter& painter) const
  {
    const QPoint cursorPos (ViewUtil::toQPoint (this->self->cursorPosition ()));

    QPen pen (this->self->config ().get<Color> ("editor/on-screen-color").qColor ());
    pen.setCapStyle (Qt::FlatCap);
    pen.setWidth (this->trimMode == TrimMode::Normal ? 2 : this->widthEdit.value ());

    painter.setPen (pen);

    if (this->points.empty () == false)
    {
      for (unsigned int i = 0; i < this->points.size () - 1; i++)
      {
        painter.drawLine (ViewUtil::toQPoint (this->points[i + 0]),
                          ViewUtil::toQPoint (this->points[i + 1]));
      }
      painter.drawLine (ViewUtil::toQPoint (this->points[this->points.size () - 1]), cursorPos);
    }
  }
};

DELEGATE_TOOL (ToolTrimMesh)
DELEGATE_TOOL_RUN_MOVE_EVENT (ToolTrimMesh)
DELEGATE_TOOL_RUN_PRESS_EVENT (ToolTrimMesh)
DELEGATE_TOOL_RUN_RELEASE_EVENT (ToolTrimMesh)
DELEGATE_TOOL_RUN_MOUSE_WHEEL_EVENT (ToolTrimMesh)
DELEGATE_TOOL_RUN_PAINT (ToolTrimMesh)
