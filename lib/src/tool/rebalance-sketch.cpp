/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QMouseEvent>
#include "render-mode.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "view/tool-tip.hpp"
#include "sketch/mesh.hpp"
#include "sketch/node-intersection.hpp"

struct ToolRebalanceSketch::Impl {
  ToolRebalanceSketch* self;
  const bool           renderWireframe;

  Impl (ToolRebalanceSketch* s) 
    : self            (s)
    , renderWireframe (s->state ().scene ().commonRenderMode ().renderWireframe ())
  {
    this->self->renderMirror (false);
    this->self->state ().scene ().renderWireframe (true);

    ViewToolTip toolTip;
    toolTip.add (ViewToolTip::MouseEvent::Left, QObject::tr ("Set new root"));
    this->self->showToolTip (toolTip);
  }

  ToolResponse runMouseReleaseEvent (const QMouseEvent& e) {
    if (e.button () == Qt::LeftButton) {
      SketchNodeIntersection intersection;
      if (this->self->intersectsScene (e, intersection)) {
        this->self->snapshotSketchMeshes ();
        intersection.mesh ().rebalance (intersection.node ());
        return ToolResponse::Redraw;
      }
    }
    return ToolResponse::None;
  }

  void runClose () {
    this->self->state ().scene ().renderWireframe (this->renderWireframe);
  }
};

DELEGATE_TOOL                         (ToolRebalanceSketch)
DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT (ToolRebalanceSketch)
DELEGATE_TOOL_RUN_CLOSE               (ToolRebalanceSketch)
