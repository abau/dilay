/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCheckBox>
#include <QSlider>
#include "cache.hpp"
#include "mirror.hpp"
#include "primitive/plane.hpp"
#include "scene.hpp"
#include "sketch/bone-intersection.hpp"
#include "sketch/mesh.hpp"
#include "sketch/node-intersection.hpp"
#include "state.hpp"
#include "tool/util/movement.hpp"
#include "tool/util/scaling.hpp"
#include "tools.hpp"
#include "view/pointing-event.hpp"
#include "view/tool-tip.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

struct ToolModifySketch::Impl
{
  ToolModifySketch* self;
  SketchMesh*       mesh;
  SketchNode*       node;
  SketchNode*       parent;
  ToolUtilMovement  movement;
  ToolUtilScaling   scaling;
  bool              transformChildren;
  bool              snap;
  QSlider&          snapWidthEdit;

  Impl (ToolModifySketch* s)
    : self (s)
    , mesh (nullptr)
    , node (nullptr)
    , parent (nullptr)
    , movement (s->state ().camera (), false)
    , scaling (s->state ().camera ())
    , transformChildren (s->cache ().get<bool> ("transform-children", false))
    , snap (s->cache ().get<bool> ("snap", true))
    , snapWidthEdit (ViewUtil::slider (1, s->cache ().get<int> ("snap-width", 5), 10))
  {
  }

  ToolResponse runInitialize ()
  {
    this->self->renderMirror (true);

    this->setupProperties ();
    this->setupToolTip ();

    return ToolResponse::None;
  }

  void setupProperties ()
  {
    ViewTwoColumnGrid& properties = this->self->properties ();

    this->self->addMirrorProperties (false);
    this->self->addAlongPrimaryPlaneProperties (this->movement);

    QCheckBox& transformCEdit =
      ViewUtil::checkBox (QObject::tr ("Transform children"), this->transformChildren);
    ViewUtil::connect (transformCEdit, [this](bool m) {
      this->transformChildren = m;
      this->self->cache ().set ("transform-children", m);
    });
    properties.add (transformCEdit);

    QCheckBox& snapEdit = ViewUtil::checkBox (QObject::tr ("Snap"), this->snap);
    ViewUtil::connect (snapEdit, [this](bool s) {
      this->snap = s;
      this->snapWidthEdit.setEnabled (s);
      this->self->cache ().set ("snap", s);
    });
    properties.add (snapEdit);

    this->snapWidthEdit.setEnabled (this->snap);
    ViewUtil::connect (this->snapWidthEdit,
                       [this](int w) { this->self->cache ().set ("snap-width", w); });
    properties.addStacked (QObject::tr ("Snap width"), this->snapWidthEdit);
  }

  void setupToolTip ()
  {
    ViewToolTip toolTip;
    toolTip.add (ViewInput::Event::MouseLeft, QObject::tr ("Drag node to move"));
    toolTip.add (ViewInput::Event::MouseLeft, ViewInput::Modifier::Shift,
                 QObject::tr ("Drag node to scale"));
    toolTip.add (ViewInput::Event::MouseLeft, ViewInput::Modifier::Ctrl,
                 QObject::tr ("Drag node to add child"));
    this->self->showToolTip (toolTip);
  }

  ToolResponse runMoveEvent (const ViewPointingEvent& e)
  {
    if (e.leftButton () && this->node)
    {
      if (e.modifiers () == Qt::ShiftModifier)
      {
        if (this->scaling.move (e))
        {
          if (this->parent)
          {
            this->mesh->scale (*this->parent, this->scaling.factor (), false,
                               this->self->mirrorDimension ());
          }
          this->mesh->scale (*this->node, this->scaling.factor (), this->transformChildren,
                             this->self->mirrorDimension ());
        }
      }
      else if (this->movement.move (e))
      {
        if (this->parent)
        {
          this->mesh->move (*this->parent, this->movement.delta (), false,
                            this->self->mirrorDimension ());
        }
        this->mesh->move (*this->node, this->movement.delta (), this->transformChildren,
                          this->self->mirrorDimension ());
      }
      return ToolResponse::Redraw;
    }
    else
    {
      return ToolResponse::None;
    }
  }

  ToolResponse runPressEvent (const ViewPointingEvent& e)
  {
    auto handleNodeIntersection = [this, &e](SketchNodeIntersection& intersection) {
      this->self->snapshotSketchMeshes ();

      this->movement.reset (intersection.position ());
      this->scaling.reset (intersection.node ().data ().center (), intersection.position ());

      this->mesh = &intersection.mesh ();
      this->parent = nullptr;

      if (e.modifiers () == Qt::ControlModifier)
      {
        SketchNode& iNode = intersection.node ();

        const float radius =
          iNode.numChildren () > 0 ? iNode.lastChild ().data ().radius () : iNode.data ().radius ();

        this->node = &this->mesh->addChild (iNode, iNode.data ().center (), radius,
                                            this->self->mirrorDimension ());
      }
      else
      {
        this->node = &intersection.node ();
      }
    };

    auto handleBoneIntersection = [this, &e](SketchBoneIntersection& intersection) {
      this->self->snapshotSketchMeshes ();

      this->movement.reset (intersection.position ());
      this->scaling.reset (intersection.projectedPosition (), intersection.position ());

      this->mesh = &intersection.mesh ();

      if (e.modifiers () == Qt::ControlModifier)
      {
        const float radius =
          glm::distance (intersection.projectedPosition (), intersection.position ());

        this->parent = nullptr;
        this->node =
          &this->mesh->addParent (intersection.child (), intersection.projectedPosition (), radius,
                                  this->self->mirrorDimension ());
      }
      else
      {
        this->node = &intersection.child ();
        this->parent = &intersection.parent ();
      }
    };

    if (e.leftButton ())
    {
      SketchNodeIntersection nodeIntersection;
      SketchBoneIntersection boneIntersection;

      if (this->self->intersectsScene (e, nodeIntersection))
      {
        handleNodeIntersection (nodeIntersection);
      }
      else if (this->self->intersectsScene (e, boneIntersection))
      {
        handleBoneIntersection (boneIntersection);
      }
    }
    return ToolResponse::None;
  }

  ToolResponse runReleaseEvent (const ViewPointingEvent& e)
  {
    bool redraw = false;

    if (e.leftButton ())
    {
      if (this->snap && this->mesh && this->self->hasMirror ())
      {
        PrimPlane mirrorPlane = this->mesh->mirrorPlane (*this->self->mirrorDimension ());

        const auto isSnappable = [this, &mirrorPlane](const SketchNode& node) -> bool {
          return mirrorPlane.absDistance (node.data ().center ()) <=
                 (this->snapWidthEdit.value ()) * this->self->mirror ().width ();
        };

        if (this->node && isSnappable (*this->node))
        {
          this->mesh->snap (*this->node, *this->self->mirrorDimension ());
          redraw = true;
        }
        if (this->parent && isSnappable (*this->parent))
        {
          this->mesh->snap (*this->parent, *this->self->mirrorDimension ());
          redraw = true;
        }
      }
      this->mesh = nullptr;
      this->node = nullptr;
      this->parent = nullptr;
    }
    return redraw ? ToolResponse::Redraw : ToolResponse::None;
  }
};

DELEGATE_TOOL (ToolModifySketch)
DELEGATE_TOOL_RUN_MOVE_EVENT (ToolModifySketch)
DELEGATE_TOOL_RUN_PRESS_EVENT (ToolModifySketch)
DELEGATE_TOOL_RUN_RELEASE_EVENT (ToolModifySketch)
