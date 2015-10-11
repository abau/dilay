/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCheckBox>
#include <QMouseEvent>
#include "cache.hpp"
#include "dimension.hpp"
#include "scene.hpp"
#include "sketch/mesh.hpp"
#include "sketch/node-intersection.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "view/properties.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"

struct ToolDeleteSketchNode::Impl {
  ToolDeleteSketchNode* self;
  bool                  deleteMesh;
  bool                  deleteChildren;

  Impl (ToolDeleteSketchNode* s) 
    : self           (s)
    , deleteMesh     (s->cache ().get <bool> ("delete-mesh"    , false))
    , deleteChildren (s->cache ().get <bool> ("delete-children", false))
  {
    this->self->renderMirror (false);

    this->setupProperties ();
    this->setupToolTip    ();
  }

  void setupProperties () {
    ViewPropertiesPart& properties = this->self->properties ().body ();

    QCheckBox& deleteChildrenEdit = ViewUtil::checkBox ( QObject::tr ("Delete children")
                                                       , this->deleteChildren );
    ViewUtil::connect (deleteChildrenEdit, [this] (bool m) {
      this->deleteChildren = m;
      this->self->cache ().set ("delete-children", m);
    });
    deleteChildrenEdit.setEnabled (!this->deleteMesh);

    QCheckBox& mirrorEdit = ViewUtil::checkBox ( QObject::tr ("Mirror")
                                               , this->self->hasMirror () );
    ViewUtil::connect (mirrorEdit, [this] (bool m) {
      this->self->mirror (m);
    });
    mirrorEdit.setEnabled (!this->deleteMesh);

    QCheckBox& deleteMeshEdit = ViewUtil::checkBox ( QObject::tr ("Delete mesh")
                                                   , this->deleteMesh );
    ViewUtil::connect (deleteMeshEdit, 
      [this, &deleteChildrenEdit, &mirrorEdit] (bool m)
    {
      this->deleteMesh = m;
      this->self->cache ().set ("delete-mesh", m);

      deleteChildrenEdit.setEnabled (!m);
      mirrorEdit        .setEnabled (!m);
    });
    properties.add (deleteMeshEdit);
    properties.add (deleteChildrenEdit);
    properties.add (mirrorEdit);
  }

  void setupToolTip () {
    ViewToolTip toolTip;
    toolTip.add (ViewToolTip::MouseEvent::Left, QObject::tr ("Delete selection"));
    this->self->showToolTip (toolTip);
  }

  ToolResponse runMouseReleaseEvent (const QMouseEvent& e) {
    if (e.button () == Qt::LeftButton) {
      SketchNodeIntersection intersection;
      if (this->self->intersectsScene (e, intersection)) {
        Scene& scene = this->self->state ().scene ();
        this->self->snapshotSketchMeshes ();

        if (this->deleteMesh) {
          scene.deleteMesh (intersection.mesh ());
        }
        else {
          intersection.mesh ().deleteNode ( intersection.node ()
                                          , this->deleteChildren
                                          , this->self->mirrorDimension () );
          if (intersection.mesh ().isEmpty ()) {
            scene.deleteMesh (intersection.mesh ());
          }
        }
        return ToolResponse::Redraw;
      }
    }
    return ToolResponse::None;
  }
};

DELEGATE_TOOL                         (ToolDeleteSketchNode)
DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT (ToolDeleteSketchNode)
