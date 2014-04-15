#include <glm/glm.hpp>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QMouseEvent>
#include "tool/new-freeform-mesh.hpp"
#include "tool/movement.hpp"
#include "view/main-window.hpp"
#include "view/gl-widget.hpp"
#include "view/tool-options.hpp"
#include "view/vector-edit.hpp"
#include "view/properties/widget.hpp"
#include "view/util.hpp"
#include "action/new-winged-mesh.hpp"
#include "mesh-type.hpp"
#include "mesh.hpp"
#include "state.hpp"
#include "history.hpp"
#include "config.hpp"
#include "camera.hpp"
#include "primitive/sphere.hpp"
#include "primitive/ray.hpp"
#include "intersection.hpp"

struct ToolNewFreeformMesh::Impl {
  ToolNewFreeformMesh* self;
  Mesh                 mesh;
  ToolMovement         movement;
  QSpinBox*            subdivEdit;
  QDoubleSpinBox*      radiusEdit;
  ViewVectorEdit*      positionEdit;
  PrimSphere           meshSphere;

  Impl (ToolNewFreeformMesh* s) : self (s) {
    int   initSubdivisions = Config::get <int>   ("/cache/tool/new-freeform-mesh/subdivisions", 2);
    float initRadius       = Config::get <float> ("/cache/tool/new-freeform-mesh/radius", 1.0f);

    // connect subdivision edit
    this->subdivEdit = this->self->toolOptions ()->add <QSpinBox> 
                        ( QObject::tr ("Subdivisions")
                        , ViewUtil::spinBox (1, initSubdivisions, 5));
    ViewUtil::connect (this->subdivEdit, [this] (int n) { this->setMeshByInput (n); });

    // connect radius edit
    this->radiusEdit = this->self->toolOptions ()->add <QDoubleSpinBox>
                        ( QObject::tr ("Radius")
                        , ViewUtil::spinBox (0.001f, initRadius));
    ViewUtil::connect (this->radiusEdit, [this] (double r) { this->setMeshByInput (float (r)); });

    // connect position edit
    this->positionEdit = this->self->toolOptions ()->add <ViewVectorEdit>
                          (QObject::tr ("Position"), new ViewVectorEdit);
    QObject::connect (this->positionEdit, &ViewVectorEdit::vectorEdited, [this] 
        (const glm::vec3& p) { this->setMeshByInput (p); });

    // setup mesh
    this->movement.byScreenPos ( this->self->mainWindow ()->properties ()->movement ()
                               , this->self->mainWindow ()->glWidget ()->cursorPosition ());
    this->setMeshByInput       (initSubdivisions);
    this->setMeshByInput       (initRadius);
    this->setMeshByMovement    ();
    this->hover                (this->self->mainWindow ()->glWidget ()->cursorPosition ());
  }

  ~Impl () {
    State::history ().add <ActionNewWingedMesh> ()->run (MeshType::Freeform, this->mesh);
  }

  static QString toolName () {
    return QObject::tr ("New Freeform Mesh");
  }

  void setMeshByInput (int numSubdivisions) {
    glm::vec3 oldPos   = this->mesh.position ();
    glm::vec3 oldScale = this->mesh.scaling  ();
    this->mesh = Mesh::icosphere (numSubdivisions);
    this->mesh.bufferData      ();
    this->mesh.position        (oldPos);
    this->mesh.scaling         (oldScale);
    this->subdivEdit->setValue (numSubdivisions);
    this->self->mainWindow ()->glWidget ()->update ();
    Config::cache <int> ("/cache/tool/new-freeform-mesh/subdivisions", numSubdivisions);
  }

  void setMeshByInput (const glm::vec3& pos) {
    this->movement.position    (pos);
    this->mesh.position        (pos);
    this->meshSphere.center    (pos);
    this->positionEdit->vector (pos);
    this->self->mainWindow ()->glWidget ()->update ();
  }

  void setMeshByInput (float radius) {
    this->mesh.scaling         (glm::vec3 (radius));
    this->meshSphere.radius    (radius);
    this->radiusEdit->setValue (radius);
    this->self->mainWindow ()->glWidget ()->update ();
    Config::cache <float> ("/cache/tool/new-freeform-mesh/radius", radius);
  }

  void setMeshByMovement () {
    this->mesh.position        (this->movement.position ());
    this->meshSphere.center    (this->movement.position ());
    this->positionEdit->vector (this->movement.position ());
  }

  void runRender () {
    this->mesh.render ();
  }
  
  void hover (const glm::ivec2& pos) {
    this->self->hover (IntersectionUtil::intersects ( State::camera ().ray (pos)
                                                    , this->meshSphere, nullptr));
  }

  bool runMouseMoveEvent (QMouseEvent* e) {
    if (this->self->isDraged ()) {
      if (this->movement.byMouseEvent (this->self->mainWindow ()->properties ()->movement (), e)) {
        this->setMeshByMovement ();
        return true;
      }
      else if (e->modifiers ().testFlag (Qt::ControlModifier)) {
        glm::vec3 radiusPoint;
        if (this->movement.onCameraPlane (ViewUtil::toIVec2 (*e), &radiusPoint)) {
          this->setMeshByInput (glm::distance (radiusPoint, this->movement.position ()));
        }
      }
    }
    else {
      this->hover (ViewUtil::toIVec2 (*e));
    }
    return false;
  }

  bool runMousePressEvent (QMouseEvent*) {
    this->self->dragIfHovered ();
    return false;
  }

  bool runMouseReleaseEvent (QMouseEvent*) {
    this->self->hoverIfDraged ();
    return false;
  }

  bool runWheelEvent (QWheelEvent* e) {
    if (e->modifiers ().testFlag (Qt::ControlModifier)) {
      if (e->orientation () == Qt::Vertical) {
        if (e->delta () > 0)
          this->setMeshByInput (this->subdivEdit->value () + 1);
        else if (e->delta () < 0)
          this->setMeshByInput (this->subdivEdit->value () - 1);
      }
      return true;
    }
    return false;
  }
};

DELEGATE_TOOL   (ToolNewFreeformMesh)
DELEGATE        (void, ToolNewFreeformMesh, runRender)
DELEGATE1       (bool, ToolNewFreeformMesh, runMouseMoveEvent, QMouseEvent*)
DELEGATE1       (bool, ToolNewFreeformMesh, runMousePressEvent, QMouseEvent*)
DELEGATE1       (bool, ToolNewFreeformMesh, runMouseReleaseEvent, QMouseEvent*)
DELEGATE1       (bool, ToolNewFreeformMesh, runWheelEvent, QWheelEvent*)
