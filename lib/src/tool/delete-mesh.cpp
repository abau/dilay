#include <QMouseEvent>
#include "action/delete-winged-mesh.hpp"
#include "history.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "winged/face-intersection.hpp"

struct ToolDeleteMesh::Impl {
  ToolDeleteMesh* self;

  Impl (ToolDeleteMesh* s) : self (s) {}

  bool runAllowUndoRedo () const {
    return true;
  }

  ToolResponse runMouseReleaseEvent (const QMouseEvent& e) {
    if (e.button () == Qt::LeftButton) {
      WingedFaceIntersection intersection;
      if (this->self->intersectsScene (e, intersection)) {
        this->self->state ().history ().add <ActionDeleteWMesh> ()
                                       .run ( this->self->state ().scene ()
                                            , intersection.mesh () );
        return ToolResponse::Redraw;
      }
    }
    return ToolResponse::None;
  }
};

DELEGATE_TOOL                         (ToolDeleteMesh)
DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT (ToolDeleteMesh)
