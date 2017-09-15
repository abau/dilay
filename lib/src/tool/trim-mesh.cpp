/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QAbstractButton>
#include <QButtonGroup>
#include <QPainter>
#include "cache.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "config.hpp"
#include "dynamic/mesh.hpp"
#include "history.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool/trim-mesh/action.hpp"
#include "tool/trim-mesh/border.hpp"
#include "tool/trim-mesh/split-mesh.hpp"
#include "tools.hpp"
#include "view/double-slider.hpp"
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
  float                   width;

  Impl (ToolTrimMesh* s)
    : self (s)
    , trimMode (TrimMode (s->cache ().get<int> ("trim-mode", int(TrimMode::Normal))))
    , width (s->cache ().get<float> ("trim-width", 0.1f))
  {
  }

  void setupProperties ()
  {
    ViewTwoColumnGrid& properties = this->self->properties ();

    ViewDoubleSlider& widthEdit = ViewUtil::slider (2, 0.01f, this->width, 1.0f);
    ViewUtil::connect (widthEdit, [this](float w) {
      this->width = w;
      this->self->cache ().set ("trim-width", w);
    });

    QButtonGroup& trimModeEdit = *new QButtonGroup;
    properties.add (trimModeEdit,
                    {QObject::tr ("Normal"), QObject::tr ("Slice"), QObject::tr ("Cut")});
    ViewUtil::connect (trimModeEdit, [this, &widthEdit](int id) {
      this->trimMode = TrimMode (id);
      this->self->cache ().set ("trim-mode", id);
      widthEdit.setEnabled (this->trimMode != TrimMode::Normal);
    });
    trimModeEdit.button (int(this->trimMode))->click ();

    properties.addStacked (QObject::tr ("Width"), widthEdit);
  }

  void setupToolTip ()
  {
    ViewToolTip toolTip;
    toolTip.add (ViewToolTip::Event::MouseLeft, QObject::tr ("Drag to trim"));
    this->self->showToolTip (toolTip);
  }

  ToolResponse runInitialize ()
  {
    this->setupProperties ();
    this->setupToolTip ();

    return ToolResponse::None;
  }

  ToolResponse runMoveEvent (const ViewPointingEvent&)
  {
    return this->points.empty () ? ToolResponse::None : ToolResponse::Redraw;
  }

  ToolResponse runPressEvent (const ViewPointingEvent&) { return ToolResponse::None; }

  TrimStatus trimMesh (DynamicMesh& mesh, float offset, bool reverse)
  {
    ToolTrimMeshBorder border (this->self->state ().camera (), this->points, offset, reverse);
    if (ToolTrimMeshSplitMesh::splitMesh (mesh, border))
    {
      if (border.hasVertices ())
      {
        if (ToolTrimMeshAction::trimMesh (mesh, border))
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

  TrimStatus trimMesh (DynamicMesh& mesh)
  {
    if (this->trimMode == TrimMode::Normal)
    {
      return this->trimMesh (mesh, 0.0f, false);
    }
    else if (this->trimMode == TrimMode::Slice)
    {
      TrimStatus status = this->trimMesh (mesh, this->width, false);
      if (status == TrimStatus::Failed)
      {
        return status;
      }
      else
      {
        return this->trimMesh (mesh, this->width, true);
      }
    }
    else if (this->trimMode == TrimMode::Cut)
    {
      DynamicMesh& mesh2 =
        this->self->state ().scene ().newDynamicMesh (this->self->config (), mesh);
      TrimStatus status = this->trimMesh (mesh, -this->width, false);

      if (status == TrimStatus::Trimmed)
      {
        return this->trimMesh (mesh2, -this->width, true);
      }
      else
      {
        this->self->state ().scene ().deleteMesh (mesh2);
        return status;
      }
    }
    else
    {
      DILAY_IMPOSSIBLE
    }
  }

  ToolResponse runReleaseEvent (const ViewPointingEvent& e)
  {
    if (e.primaryButton () == false)
    {
      return ToolResponse::None;
    }
    else
    {
      this->points.push_back (e.ivec2 ());

      if (this->points.size () >= 2)
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
    QPen         pen (this->self->config ().get<Color> ("editor/on-screen-color").qColor ());

    pen.setWidth (2);
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
DELEGATE_TOOL_RUN_PAINT (ToolTrimMesh)
