/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCheckBox>
#include <QFrame>
#include <QMouseEvent>
#include <QPushButton>
#include "cache.hpp"
#include "dimension.hpp"
#include "scene.hpp"
#include "sketch/mesh.hpp"
#include "sketch/node-intersection.hpp"
#include "state.hpp"
#include "tool/util/movement.hpp"
#include "tool/util/scaling.hpp"
#include "tools.hpp"
#include "view/properties.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"

struct ToolModifySketch::Impl {
  ToolModifySketch* self;
  SketchMesh*       mesh;
  SketchNode*       node;
  ToolUtilMovement  movement;
  ToolUtilScaling   scaling;
  bool              mirror;
  bool              moveChildren;

  Impl (ToolModifySketch* s)
    : self         (s)
    , mesh         (nullptr)
    , node         (nullptr)
    , movement     ( s->state ().camera ()
                   , s->cache ().getFrom <MovementConstraint> ( "constraint"
                                                              , MovementConstraint::CameraPlane ))
    , scaling      (s->state ().camera ())
    , mirror       (s->cache ().get <bool> ("mirror", true))
    , moveChildren (s->cache ().get <bool> ("move-children", false))
  {
    this->setupProperties ();
    this->setupToolTip    ();
  }

  const Dimension* mirrorDimension () const {
    static Dimension dim = Dimension::X;

    return this->mirror ? &dim : nullptr;
  }

  void mirrorSketchMeshes () {
    assert (this->mirror);

    this->self->snapshotSketchMeshes ();

    this->self->state ().scene ().forEachMesh (
      [this] (SketchMesh& mesh) {
        mesh.mirror (*this->mirrorDimension ());
      }
    );
  }

  void setupProperties () {
    ViewPropertiesPart& properties = this->self->properties ().body ();

    properties.add (QObject::tr ("Move along"), ViewUtil::emptyWidget ());
    this->movement.addProperties (properties, [this] () {
      this->self->cache ().set ("constraint", this->movement.constraint ());
    });
    properties.add (ViewUtil::horizontalLine ());

    QPushButton& syncButton = ViewUtil::pushButton (QObject::tr ("Sync"));
    ViewUtil::connect (syncButton, [this] () {
      this->mirrorSketchMeshes ();
      this->self->updateGlWidget ();
    });
    syncButton.setEnabled (bool (this->mirror));

    QCheckBox& mirrorEdit = ViewUtil::checkBox (QObject::tr ("Mirror"), this->mirror);
    ViewUtil::connect (mirrorEdit, [this, &syncButton] (bool m) {
      this->mirror = m;
      syncButton.setEnabled (m);
      this->self->cache ().set ("mirror", m);
    });
    properties.add (mirrorEdit, syncButton);

    QCheckBox& moveCEdit = ViewUtil::checkBox (QObject::tr ("Move children"), this->moveChildren);
    ViewUtil::connect (moveCEdit, [this] (bool m) {
      this->moveChildren = m;
      this->self->cache ().set ("move-children", m);
    });
    properties.add (moveCEdit);
  }

  void setupToolTip () {
    ViewToolTip toolTip;
    toolTip.add ( ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to move"));
    toolTip.add ( ViewToolTip::MouseEvent::Left, ViewToolTip::Modifier::Shift
                , QObject::tr ("Drag to scale") );
    toolTip.add ( ViewToolTip::MouseEvent::Left, ViewToolTip::Modifier::Ctrl
                , QObject::tr ("Drag to add new node") );
    this->self->showToolTip (toolTip);
  }

  ToolResponse runMouseMoveEvent (const QMouseEvent& e) {
    if (e.buttons () == Qt::LeftButton && this->node) {
      if (e.modifiers () == Qt::ShiftModifier) {
        if (this->scaling.move (e)) {
          this->mesh->radius (*this->node, this->scaling.delta (), this->mirrorDimension ());
        }
      }
      else if (this->movement.move (e, false)) {
        this->mesh->move (*this->node, this->movement.delta ()
                         , this->moveChildren, this->mirrorDimension () );
      }
      return ToolResponse::Redraw;
    }
    else {
      return ToolResponse::None;
    }
  }

  ToolResponse runMousePressEvent (const QMouseEvent& e) {
    if (e.button () == Qt::LeftButton) {
      SketchNodeIntersection intersection;
      if (this->self->intersectsScene (e, intersection)) {
        this->self->snapshotSketchMeshes ();

        this->movement.resetPosition ( intersection.position ());
        this->scaling .resetPosition ( intersection.node ().data ().position ()
                                     , intersection.position () );

        this->mesh = &intersection.mesh ();
        
        if (e.modifiers () == Qt::ControlModifier) {
          SketchNode& iNode = intersection.node ();

          const float radius = iNode.numChildren () > 0
                             ? iNode.lastChild ().data ().radius ()
                             : iNode.data ().radius ();

          this->node = &this->mesh->addChild ( iNode, iNode.data ().position ()
                                             , radius, this->mirrorDimension () );
        }
        else {
          this->node = &intersection.node ();
        }
      }
    }
    return ToolResponse::None;
  }

  ToolResponse runMouseReleaseEvent (const QMouseEvent& e) {
    if (e.button () == Qt::LeftButton) {
      this->mesh = nullptr;
      this->node = nullptr;
    }
    return ToolResponse::None;
  }
};

DELEGATE_TOOL                         (ToolModifySketch)
DELEGATE_TOOL_RUN_MOUSE_MOVE_EVENT    (ToolModifySketch)
DELEGATE_TOOL_RUN_MOUSE_PRESS_EVENT   (ToolModifySketch)
DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT (ToolModifySketch)
