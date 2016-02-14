/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QObject>
#include "scene.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "view/pointing-event.hpp"
#include "view/tool-tip.hpp"
#include "winged/face-intersection.hpp"

struct ToolDeleteMesh::Impl {
  ToolDeleteMesh* self;

  Impl (ToolDeleteMesh* s) 
    : self (s)
  {
    ViewToolTip toolTip;
    toolTip.add (ViewToolTip::MouseEvent::Left, QObject::tr ("Delete selection"));
    this->self->showToolTip (toolTip);
  }

  ToolResponse runReleaseEvent (const ViewPointingEvent& e) {
    if (e.primaryButton ()) {
      WingedFaceIntersection intersection;
      if (this->self->intersectsScene (e, intersection)) {
        Scene& scene = this->self->state ().scene ();
        this->self->snapshotWingedMeshes ();
        scene.deleteMesh (intersection.mesh ());
        return ToolResponse::Redraw;
      }
    }
    return ToolResponse::None;
  }
};

DELEGATE_TOOL                   (ToolDeleteMesh)
DELEGATE_TOOL_RUN_RELEASE_EVENT (ToolDeleteMesh)
