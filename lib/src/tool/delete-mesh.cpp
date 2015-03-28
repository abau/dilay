#include <QMouseEvent>
#include "history.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tools.hpp"
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

  ToolResponse runMouseReleaseEvent (const QMouseEvent& e) {
    if (e.button () == Qt::LeftButton) {
      WingedFaceIntersection intersection;
      if (this->self->intersectsScene (e, intersection)) {
        this->self->snapshotScene ();
        this->self->state ().scene ().deleteMesh (intersection.mesh ());
        return ToolResponse::Redraw;
      }
    }
    return ToolResponse::None;
  }
};

DELEGATE_TOOL                         (ToolDeleteMesh)
DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT (ToolDeleteMesh)
