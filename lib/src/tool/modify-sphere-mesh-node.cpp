#include <QMouseEvent>
#include "tool/modify-sphere-mesh-node.hpp"
#include "tool/util/position.hpp"
#include "tool/util/radius.hpp"
#include "view/main-window.hpp"
#include "view/gl-widget.hpp"
#include "view/util.hpp"
#include "view/vector-edit.hpp"
#include "mesh-type.hpp"
#include "mesh.hpp"
#include "state.hpp"
#include "history.hpp"
#include "config.hpp"
#include "primitive/sphere.hpp"
#include "primitive/ray.hpp"
#include "intersection.hpp"
#include "camera.hpp"
#include "sphere/mesh.hpp"
#include "action/new-sphere-mesh.hpp"
#include "action/modify-sphere-mesh.hpp"

struct ToolModifySphereMeshNode::Impl {
  ToolModifySphereMeshNode*      self;
  ToolModifySphereMeshNode::Mode mode;
  ConfigProxy                    config;
  ToolUtilPosition               posUtil;
  ToolUtilRadius                 radiusUtil;
  Mesh                           mesh;
  PrimSphere                     meshGeometry;

  Impl (ToolModifySphereMeshNode* s, Mode m) 
    : self       (s) 
    , mode       (m)
    , config     ("/cache/tool/modify-sphere-mesh-node/")
    , posUtil    (*s)
    , radiusUtil (*s, this->config, this->posUtil.movement ())
  {
    QObject::connect (&this->posUtil.vectorEdit (), &ViewVectorEdit::vectorEdited,
      [this] (const glm::vec3&) { this->updateMesh (); });

    ViewUtil::connect (this->radiusUtil.radiusEdit (), 
      [this] (float) { this->updateMesh (); });

    SphereMeshNode::setupMesh (this->mesh);
    this->updateMesh          ();
    this->hover               (this->self->mainWindow ().glWidget ().cursorPosition ());
  }

  ~Impl () {
    if (this->mode == ToolModifySphereMeshNode::Mode::NewMesh) {
      State::history ().add <ActionNewSphereMesh> ()->run ( this->meshGeometry.center ()
                                                          , this->meshGeometry.radius ());
    }
  }

  static QString toolName (ToolModifySphereMeshNode::Mode mode) {
    switch (mode) {
      case ToolModifySphereMeshNode::Mode::NewMesh:
        return QObject::tr ("New Mesh");
      case ToolModifySphereMeshNode::Mode::NewNode:
        return QObject::tr ("New Node");
      case ToolModifySphereMeshNode::Mode::ModifyNode:
        return QObject::tr ("Modify Node");
    }
    assert (false);
  }

  void updateMesh () {
    this->mesh.position        (this->posUtil.position  ());
    this->mesh.scaling         (glm::vec3 (this->radiusUtil.radius ()));
    this->meshGeometry.center  (this->posUtil.position  ());
    this->meshGeometry.radius  (this->radiusUtil.radius ());
    this->self->updateGlWidget ();
  }

  void runRender () {
    this->mesh.render ();
  }
  
  void hover (const glm::ivec2& pos) {
    this->self->hover (IntersectionUtil::intersects ( State::camera ().ray (pos)
                                                    , this->meshGeometry, nullptr));
  }

  bool runMouseMoveEvent (QMouseEvent& e) {
    if (this->self->isDraged ()) {
      if (e.modifiers ().testFlag (Qt::ControlModifier)) {
        if (this->radiusUtil.runMouseMoveEvent (e)) {
          this->updateMesh ();
        }
      }
      else if (this->posUtil.runMouseMoveEvent (e)) {
        this->updateMesh ();
      }
    }
    else {
      this->hover (ViewUtil::toIVec2 (e));
    }
    return false;
  }
};

DELEGATE_BIG3_BASE ( ToolModifySphereMeshNode, (ViewMainWindow& w, QContextMenuEvent& e, Mode m)
                   , (this,m), Tool, (w,e,toolName (m)))
DELEGATE1_STATIC (QString, ToolModifySphereMeshNode, toolName, Mode)
DELEGATE        (void    , ToolModifySphereMeshNode, runRender)
DELEGATE1       (bool    , ToolModifySphereMeshNode, runMouseMoveEvent, QMouseEvent&)
