/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QMouseEvent>
#include <glm/glm.hpp>
#include "cache.hpp"
#include "state.hpp"
#include "tool/util/movement.hpp"
#include "tools.hpp"
#include "view/properties.hpp"
#include "view/tool-tip.hpp"
#include "winged/mesh.hpp"
#include "winged/face-intersection.hpp"

struct ToolMoveMesh::Impl {
  ToolMoveMesh*    self;
  WingedMesh*      mesh;
  ToolUtilMovement movement;

  Impl (ToolMoveMesh* s) 
    : self     (s) 
    , mesh     (nullptr)
    , movement ( s->state ().camera ()
               , s->cache ().getFrom <MovementConstraint> ( "constraint"
                                                          , MovementConstraint::CameraPlane ))
  {
    this->setupProperties ();
    this->setupToolTip    ();
  }

  void setupProperties () {
    this->movement.addProperties ( this->self->properties ().body ()
                                 , [this] ()
    {
      this->setupToolTip ();
      this->self->cache ().set ("constraint", this->movement.constraint ());
    });
  }

  void setupToolTip () {
    ViewToolTip toolTip;
    toolTip.add (ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to move"));

    if (this->movement.constraint () != MovementConstraint::CameraPlane) {
      toolTip.add ( ViewToolTip::MouseEvent::Left, ViewToolTip::Modifier::Shift
                  , QObject::tr ("Drag to move orthogonally") );
    }
    this->self->showToolTip (toolTip);
  }

  ToolResponse runMouseMoveEvent (const QMouseEvent& e) {
    if (e.buttons () == Qt::LeftButton && this->mesh && this->movement.move (e, true)) {
      this->mesh->translate  (this->movement.delta ());
      return ToolResponse::Redraw;
    }
    else {
      return ToolResponse::None;
    }
  }

  ToolResponse runMousePressEvent (const QMouseEvent& e) {
    if (e.button () == Qt::LeftButton) {
      WingedFaceIntersection intersection;
      if (this->self->intersectsScene (e, intersection)) {
        this->mesh = &intersection.mesh ();
        this->movement.resetPosition (intersection.position ());

        this->self->snapshotWingedMeshes ();
      }
    }
    return ToolResponse::None;
  }

  ToolResponse runMouseReleaseEvent (const QMouseEvent& e) {
    if (e.button () == Qt::LeftButton && this->mesh) {
      this->mesh->normalize  ();
      this->mesh->bufferData ();
      this->mesh = nullptr;
    }
    return ToolResponse::None;
  }
};

DELEGATE_TOOL                         (ToolMoveMesh)
DELEGATE_TOOL_RUN_MOUSE_MOVE_EVENT    (ToolMoveMesh)
DELEGATE_TOOL_RUN_MOUSE_PRESS_EVENT   (ToolMoveMesh)
DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT (ToolMoveMesh)
