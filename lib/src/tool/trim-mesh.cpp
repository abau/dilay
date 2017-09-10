/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QPainter>
#include "camera.hpp"
#include "color.hpp"
#include "config.hpp"
#include "dynamic/mesh-intersection.hpp"
#include "dynamic/mesh.hpp"
#include "history.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool/trim-mesh/action.hpp"
#include "tool/trim-mesh/border.hpp"
#include "tool/trim-mesh/split-mesh.hpp"
#include "tools.hpp"
#include "view/main-window.hpp"
#include "view/pointing-event.hpp"
#include "view/util.hpp"

struct ToolTrimMesh::Impl
{
  ToolTrimMesh*           self;
  std::vector<glm::ivec2> points;

  Impl (ToolTrimMesh* s)
    : self (s)
  {
  }

  ToolResponse runInitialize () { return ToolResponse::None; }

  ToolResponse runMoveEvent (const ViewPointingEvent&)
  {
    return this->points.empty () ? ToolResponse::None : ToolResponse::Redraw;
  }

  ToolResponse runPressEvent (const ViewPointingEvent&) { return ToolResponse::None; }

  ToolResponse runReleaseEvent (const ViewPointingEvent& e)
  {
    if (e.primaryButton () == false)
    {
      return ToolResponse::None;
    }
    else
    {
      DynamicMeshIntersection intersection;

      this->points.push_back (e.ivec2 ());

      if (this->points.size () >= 2 &&
          this->self->intersectsScene (e.ivec2 (), intersection) == false)
      {
        this->self->snapshotDynamicMeshes ();

        bool intersection = false;
        bool failed = false;

        this->self->state ().scene ().forEachMesh (
          [&intersection, &failed, this](DynamicMesh& mesh) {
            ToolTrimMeshBorder border (this->self->state ().camera (), this->points);
            if (failed)
            {
              return;
            }
            else if (ToolTrimMeshSplitMesh::splitMesh (mesh, border) &&
                     ToolTrimMeshAction::trimMesh (mesh, border))
            {
              intersection = true;
            }
            else
            {
              failed = true;
            }
          });

        if (failed)
        {
          this->self->state ().undo ();
          this->self->state ().history ().dropFutureSnapshot ();
          ViewUtil::error (this->self->state ().mainWindow (),
                           QObject::tr ("Could not trim mesh."));
        }
        else if (intersection == false)
        {
          this->self->state ().history ().dropPastSnapshot ();
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
