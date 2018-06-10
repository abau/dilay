/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCheckBox>
#include <QSlider>
#include "cache.hpp"
#include "camera.hpp"
#include "mirror.hpp"
#include "primitive/plane.hpp"
#include "scene.hpp"
#include "sketch/bone-intersection.hpp"
#include "sketch/mesh.hpp"
#include "sketch/node-intersection.hpp"
#include "state.hpp"
#include "tool/util/movement.hpp"
#include "tool/util/rotation.hpp"
#include "tool/util/scaling.hpp"
#include "tools.hpp"
#include "view/pointing-event.hpp"
#include "view/tool-tip.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

struct ToolEditSketch::Impl
{
  ToolEditSketch*  self;
  SketchMesh*      mesh;
  SketchNode*      node;
  SketchNode*      parent;
  ToolUtilMovement movement;
  ToolUtilScaling  scaling;
  ToolUtilRotation rotation;
  bool             transformChildren;
  bool             splitAndJoin;
  bool             snap;
  QSlider&         snapWidthEdit;

  Impl (ToolEditSketch* s)
    : self (s)
    , mesh (nullptr)
    , node (nullptr)
    , parent (nullptr)
    , movement (s->state ().camera (), false)
    , scaling (s->state ().camera ())
    , rotation (s->state ().camera ())
    , transformChildren (s->cache ().get<bool> ("transform-children", false))
    , splitAndJoin (s->cache ().get<bool> ("split-and-join", false))
    , snap (s->cache ().get<bool> ("snap", true))
    , snapWidthEdit (ViewUtil::slider (1, s->cache ().get<int> ("snap-width", 5), 10))
  {
  }

  ToolResponse runInitialize ()
  {
    this->self->supportsMirror ();

    this->setupProperties ();
    this->setupToolTip ();

    return ToolResponse::None;
  }

  void setupProperties ()
  {
    ViewTwoColumnGrid& properties = this->self->properties ();

    this->self->addMirrorProperties ();
    this->self->enableMirrorProperties (not this->splitAndJoin);

    this->self->addMoveOnPrimaryPlaneProperties (this->movement);

    QCheckBox& transformCEdit =
      ViewUtil::checkBox (QObject::tr ("Transform children"), this->transformChildren);
    ViewUtil::connect (transformCEdit, [this](bool m) {
      this->transformChildren = m;
      this->self->cache ().set ("transform-children", m);
    });
    transformCEdit.setEnabled (not this->splitAndJoin);
    properties.add (transformCEdit);

    QCheckBox& splitAndJoinEdit =
      ViewUtil::checkBox (QObject::tr ("Split and join"), this->splitAndJoin);
    properties.add (splitAndJoinEdit);

    QCheckBox& snapEdit = ViewUtil::checkBox (QObject::tr ("Snap"), this->snap);
    ViewUtil::connect (snapEdit, [this](bool s) {
      this->snap = s;
      this->snapWidthEdit.setEnabled (s);
      this->self->cache ().set ("snap", s);
    });
    snapEdit.setEnabled (not this->splitAndJoin);
    properties.add (snapEdit);

    ViewUtil::connect (this->snapWidthEdit,
                       [this](int w) { this->self->cache ().set ("snap-width", w); });
    this->snapWidthEdit.setEnabled (this->snap && not this->splitAndJoin);
    properties.addStacked (QObject::tr ("Snap width"), this->snapWidthEdit);

    ViewUtil::connect (splitAndJoinEdit, [this, &transformCEdit, &snapEdit](bool s) {
      this->splitAndJoin = s;
      this->self->cache ().set ("split-and-join", s);
      transformCEdit.setEnabled (not this->splitAndJoin);
      snapEdit.setEnabled (not this->splitAndJoin);
      this->snapWidthEdit.setEnabled (not this->splitAndJoin);
      this->self->enableMirrorProperties (not this->splitAndJoin);
    });
  }

  void setupToolTip ()
  {
    ViewToolTip toolTip;
    toolTip.add (ViewInputEvent::MouseLeft, QObject::tr ("Add new sketch"));
    toolTip.add (ViewInputEvent::MouseLeft, QObject::tr ("Drag node to move"));
    toolTip.add (ViewInputEvent::MouseLeft, ViewInputModifier::Shift,
                 QObject::tr ("Drag node to scale"));
    toolTip.add (ViewInputEvent::MouseLeft, ViewInputModifier::Ctrl,
                 QObject::tr ("Drag node to rotate"));
    toolTip.add (ViewInputEvent::MouseLeft, ViewInputModifier::Alt,
                 QObject::tr ("Drag node to add child"));
    toolTip.add (ViewInputEvent::R, QObject::tr ("Click to set new root"));
    this->self->state ().setToolTip (&toolTip);
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
      else if (e.modifiers () == Qt::ControlModifier)
      {
        if (this->rotation.rotate (e))
        {
          this->mesh->rotate (*this->node, this->rotation.axis (), this->rotation.angle (),
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

        const bool transformChildren = this->transformChildren || this->splitAndJoin;
        this->mesh->move (*this->node, this->movement.delta (), transformChildren,
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
      this->rotation.reset (intersection.node ().data ().center ());

      this->parent = nullptr;

      if (e.modifiers () == Qt::NoModifier && this->splitAndJoin && intersection.node ().parent ())
      {
        SketchTree tree = intersection.mesh ().tree ().split (intersection.node ());

        State&      state = this->self->state ();
        SketchMesh& mesh = state.scene ().newSketchMesh (state.config (), tree);

        this->mesh = &mesh;
        this->node = &mesh.tree ().root ();
        this->self->mirrorPosition (this->mesh->tree ().root ().data ().center ());
      }
      else if (e.modifiers () == Qt::NoModifier && this->self->onKeymap ('r'))
      {
        intersection.mesh ().rebalance (intersection.node ());

        this->mesh = nullptr;
        this->node = nullptr;
        this->self->mirrorPosition (intersection.mesh ().tree ().root ().data ().center ());
      }
      else if (e.modifiers () == Qt::AltModifier)
      {
        SketchNode& iNode = intersection.node ();

        const float radius =
          iNode.numChildren () > 0 ? iNode.lastChild ().data ().radius () : iNode.data ().radius ();

        this->mesh = &intersection.mesh ();
        this->node = &this->mesh->addChild (iNode, iNode.data ().center (), radius,
                                            this->self->mirrorDimension ());
        this->self->mirrorPosition (this->mesh->tree ().root ().data ().center ());
      }
      else
      {
        this->mesh = &intersection.mesh ();
        this->node = &intersection.node ();
        this->self->mirrorPosition (this->mesh->tree ().root ().data ().center ());
      }
    };

    auto handleBoneIntersection = [this, &e](SketchBoneIntersection& intersection) {
      this->self->snapshotSketchMeshes ();

      this->movement.reset (intersection.position ());
      this->scaling.reset (intersection.projectedPosition (), intersection.position ());

      const glm::vec3 axis =
        intersection.parent ().data ().center () - intersection.child ().data ().center ();
      this->rotation.reset (intersection.projectedPosition (), axis);

      this->mesh = &intersection.mesh ();

      this->self->mirrorPosition (intersection.mesh ().tree ().root ().data ().center ());

      if (e.modifiers () == Qt::AltModifier)
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
        return ToolResponse::Redraw;
      }
      else if (this->self->intersectsScene (e, boneIntersection))
      {
        handleBoneIntersection (boneIntersection);
        return ToolResponse::Redraw;
      }
      else if (e.modifiers () == Qt::NoModifier)
      {
        this->self->snapshotSketchMeshes ();

        State&     state = this->self->state ();
        SketchTree tree;
        tree.emplaceRoot (state.camera ().viewPlaneIntersection (e.position ()), 0.1f);

        state.scene ().newSketchMesh (state.config (), tree);

        return this->runPressEvent (e);
      }
    }
    return ToolResponse::None;
  }

  ToolResponse runReleaseEvent (const ViewPointingEvent& e)
  {
    if (e.leftButton ())
    {
      if (this->splitAndJoin)
      {
        SketchNodeIntersection nodeIntersection;
        if (this->self->intersectsScene (e, nodeIntersection, (const SketchNode*) this->node))
        {
          this->self->snapshotSketchMeshes ();

          this->node->forEachChild (
            [&nodeIntersection](SketchNode& child) { nodeIntersection.node ().addChild (child); });
          this->self->state ().scene ().deleteMesh (*this->mesh);
          this->node = &nodeIntersection.node ();
          this->mesh = &nodeIntersection.mesh ();
        }
      }
      return this->runCommit ();
    }
    return ToolResponse::None;
  }

  ToolResponse runCommit ()
  {
    if (this->snap && this->mesh && this->self->mirrorEnabled ())
    {
      PrimPlane mirrorPlane = this->mesh->mirrorPlane (*this->self->mirrorDimension ());

      const auto isSnappable = [this, &mirrorPlane](const SketchNode& node) -> bool {
        return mirrorPlane.absDistance (node.data ().center ()) <=
               (this->snapWidthEdit.value ()) * this->self->mirror ().width ();
      };

      if (this->node && isSnappable (*this->node))
      {
        this->mesh->snap (*this->node, *this->self->mirrorDimension ());
      }

      if (this->parent && isSnappable (*this->parent))
      {
        this->mesh->snap (*this->parent, *this->self->mirrorDimension ());
      }
    }

    if (this->mesh)
    {
      const bool selectedRootNode = this->node->parent () == nullptr;
      const bool selectedRootBone = this->parent && this->parent->parent () == nullptr;

      if (selectedRootNode || selectedRootBone)
      {
        this->self->mirrorPosition (this->mesh->tree ().root ().data ().center ());
      }
    }

    this->mesh = nullptr;
    this->node = nullptr;
    this->parent = nullptr;

    return ToolResponse::Redraw;
  }
};

DELEGATE_TOOL (ToolEditSketch)
DELEGATE_TOOL_RUN_MOVE_EVENT (ToolEditSketch)
DELEGATE_TOOL_RUN_PRESS_EVENT (ToolEditSketch)
DELEGATE_TOOL_RUN_RELEASE_EVENT (ToolEditSketch)
DELEGATE_TOOL_RUN_COMMIT (ToolEditSketch)
