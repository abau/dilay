#include <QMouseEvent>
#include "tool/new-freeform-mesh.hpp"
#include "tool/util/integer.hpp"
#include "tool/util/position.hpp"
#include "tool/util/radius.hpp"
#include "view/main-window.hpp"
#include "view/gl-widget.hpp"
#include "view/util.hpp"
#include "view/vector-edit.hpp"
#include "action/new-winged-mesh.hpp"
#include "mesh-type.hpp"
#include "mesh.hpp"
#include "state.hpp"
#include "history.hpp"
#include "config.hpp"
#include "primitive/sphere.hpp"
#include "primitive/ray.hpp"
#include "intersection.hpp"
#include "camera.hpp"

struct ToolNewFreeformMesh::Impl {
  ToolNewFreeformMesh* self;
  ConfigProxy          config;
  ToolUtilInteger      subdivUtil;
  ToolUtilPosition     posUtil;
  ToolUtilRadius       radiusUtil;
  Mesh                 mesh;
  PrimSphere           meshGeometry;

  Impl (ToolNewFreeformMesh* s) 
    : self       (s) 
    , config     ("/cache/tool/new-freeform-mesh/")
    , subdivUtil (*s, QObject::tr ("Subdivisions"), this->config.key ("subdivisions"), 1, 2, 5)
    , posUtil    (*s)
    , radiusUtil (*s, this->config, this->posUtil.movement ())
  {
    ViewUtil::connect (this->subdivUtil.valueEdit (), 
      [this] (int) { this->updateMesh (true); });

    QObject::connect (&this->posUtil.vectorEdit (), &ViewVectorEdit::vectorEdited,
      [this] (const glm::vec3&) { this->updateMesh (); });

    ViewUtil::connect (this->radiusUtil.radiusEdit (), 
      [this] (float) { this->updateMesh (); });

    this->updateMesh (true);
    this->hover      (this->self->mainWindow ().glWidget ().cursorPosition ());
  }

  ~Impl () {
    State::history ().add <ActionNewWingedMesh> ()->run (MeshType::Freeform, this->mesh);
  }

  static QString toolName () {
    return QObject::tr ("New Mesh");
  }

  void updateMesh (bool updateSubdivision = false) {
    if (updateSubdivision) {
      this->mesh = Mesh::icosphere (this->subdivUtil.value ());
      this->mesh.bufferData        ();
      this->self->updateGlWidget   ();
    }
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
          return true;
        }
      }
      else {
        if (this->posUtil.runMouseMoveEvent (e)) {
          this->updateMesh ();
          return true;
        }
      }
    }
    else {
      this->hover (ViewUtil::toIVec2 (e));
    }
    return false;
  }

  bool runMousePressEvent (QMouseEvent&) {
    this->self->dragIfHovered ();
    return false;
  }

  bool runMouseReleaseEvent (QMouseEvent&) {
    this->self->hoverIfDraged ();
    return false;
  }

  bool runWheelEvent (QWheelEvent& e) {
    if (e.modifiers ().testFlag (Qt::ControlModifier)) {
      if (e.angleDelta ().y () > 0) {
        this->subdivUtil.increase ();
      }
      else if (e.angleDelta ().y () < 0) {
        this->subdivUtil.decrease ();
      }
      return true;
    }
    return false;
  }
};

DELEGATE_BIG3_BASE ( ToolNewFreeformMesh, (ViewMainWindow& w, QContextMenuEvent& e)
                   , (this), Tool, (w, e, toolName ()))
DELEGATE_STATIC (QString, ToolNewFreeformMesh, toolName)
DELEGATE        (void   , ToolNewFreeformMesh, runRender)
DELEGATE1       (bool   , ToolNewFreeformMesh, runMouseMoveEvent, QMouseEvent&)
DELEGATE1       (bool   , ToolNewFreeformMesh, runMousePressEvent, QMouseEvent&)
DELEGATE1       (bool   , ToolNewFreeformMesh, runMouseReleaseEvent, QMouseEvent&)
DELEGATE1       (bool, ToolNewFreeformMesh, runWheelEvent, QWheelEvent&)
