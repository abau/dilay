#include <glm/glm.hpp>
#include <QDoubleSpinBox>
#include "tool/new-sphere-mesh.hpp"
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

struct ToolNewSphereMeshNode::Impl {
  ToolNewSphereMeshNode* self;
  Mesh                   node;
  ToolMovement           movement;
  ViewVectorEdit*        positionEdit;

  Impl (ToolNewSphereMeshNode* s) : self (s) {
    float initRadius = Config::get <float> ("/cache/tool/new-sphere-mesh-node/radius", 1.0f);

    // connect radius spin box
    QDoubleSpinBox* radiusBox = this->self->toolOptions ()->add <QDoubleSpinBox>
                                  ( QObject::tr ("Radius")
                                  , ViewUtil::spinBox (0.0001f, initRadius, 0.1f, 100.f));
    void (QDoubleSpinBox::* dPtr)(double) = &QDoubleSpinBox::valueChanged;
    QObject::connect (radiusBox, dPtr, [this] (double r) { this->setNodeByInput (float (r)); });

    // connect position edit
    this->positionEdit = this->self->toolOptions ()->add <ViewVectorEdit>
                          (QObject::tr ("Position"), new ViewVectorEdit);
    QObject::connect (this->positionEdit, &ViewVectorEdit::vectorEdited, [this] 
        (const glm::vec3& p) { this->setNodeByInput (p); });

    // setup mesh
    this->movement.byScreenPos ( this->self->mainWindow ()->properties ()->movement ()
                               , this->self->mainWindow ()->glWidget ()->cursorPosition ());
    this->setNodeByInput       (initRadius);
    this->setNodeByMovement    ();
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
    this->mesh.bufferData  ();
    this->mesh.position    (oldPos);
    this->mesh.scaling     (oldScale);
    this->self->mainWindow ()->glWidget ()->update ();
    Config::set <int> ("/cache/tool/new-freeform-mesh/subdivisions", numSubdivisions);
  }

  void setMeshByInput (const glm::vec3& pos) {
    this->movement.position (pos);
    this->mesh.position     (pos);
    this->meshBox.position  (pos);
    this->self->mainWindow  ()->glWidget ()->update ();
  }

  void setMeshByInput (float radius) {
    this->mesh.scaling     (glm::vec3 (radius));
    this->meshBox.width    (2.0f * radius);
    this->self->mainWindow ()->glWidget ()->update ();
    Config::set <float> ("/cache/tool/new-freeform-mesh/radius", radius);
  }

  void setMeshByMovement () {
    this->mesh.position        (this->movement.position ());
    this->meshBox.position     (this->movement.position ());
    this->positionEdit->vector (this->movement.position ());
  }

  void runRender () {
    this->mesh.render ();
  }
  
  void hover (const QPoint& pos) {
    this->self->hover (IntersectionUtil::intersects ( State::camera ().ray (Util::toPoint (pos))
                                                    , this->meshBox));
  }

  bool runMouseMoveEvent (QMouseEvent* e) {
    if (this->self->isDraged ()) {
      if (this->movement.byMouseEvent (this->self->mainWindow ()->properties ()->movement (), e)) {
        this->setMeshByMovement ();
        return true;
      }
    }
    else {
      this->hover (e->pos ());
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
};

DELEGATE_TOOL   (ToolNewFreeformMesh)
DELEGATE        (void, ToolNewFreeformMesh, runRender)
DELEGATE1       (bool, ToolNewFreeformMesh, runMouseMoveEvent, QMouseEvent*)
DELEGATE1       (bool, ToolNewFreeformMesh, runMousePressEvent, QMouseEvent*)
DELEGATE1       (bool, ToolNewFreeformMesh, runMouseReleaseEvent, QMouseEvent*)
