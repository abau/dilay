/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QMouseEvent>
#include "mesh.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "view/tool-tip.hpp"
#include "sketch/conversion.hpp"
#include "sketch/mesh.hpp"
#include "sketch/node-intersection.hpp"

struct ToolConvertSketch::Impl {
  ToolConvertSketch* self;

  Impl (ToolConvertSketch* s) 
    : self (s)
  {
    ViewToolTip toolTip;
    toolTip.add (ViewToolTip::MouseEvent::Left, QObject::tr ("Convert selection"));
    this->self->showToolTip (toolTip);
  }

  ToolResponse runMouseReleaseEvent (const QMouseEvent& e) {
    if (e.button () == Qt::LeftButton) {
      SketchNodeIntersection intersection;
      if (this->self->intersectsScene (e, intersection)) {
        this->self->snapshotAll ();

        this->self->state ().scene ().newWingedMesh 
          ( this->self->state ().config ()
          , SketchConversion::convert (intersection.mesh (), 0.01f) );

        this->self->state ().scene ().deleteMesh (intersection.mesh ());
        return ToolResponse::Redraw;
      }
    }
    return ToolResponse::None;
  }
};

DELEGATE_TOOL                         (ToolConvertSketch)
DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT (ToolConvertSketch)
