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
  PrimSphere           meshGeometry;
  int                  numSubdivisions;

  Impl (ToolNewFreeformMesh* s) : self (s) {
    int   initSubdivisions = Config::get <int>   ("/cache/tool/new-freeform-mesh/subdivisions", 2);
    float initRadius       = Config::get <float> ("/cache/tool/new-freeform-mesh/radius", 1.0f);

    // initialize movement
    this->movement.byScreenPos ( this->self->mainWindow ()->properties ()->movement ()
                               , this->self->mainWindow ()->glWidget ()->cursorPosition ());

    // connect subdivision edit
    this->subdivEdit = this->self->toolOptions ()->add <QSpinBox> 
                        ( QObject::tr ("Subdivisions")
                        , ViewUtil::spinBox (1, initSubdivisions, 5));
    ViewUtil::connect (this->subdivEdit, [this] (int n) { this->setSubdivision (n); });

    // connect radius edit
    this->radiusEdit = this->self->toolOptions ()->add <QDoubleSpinBox>
                        ( QObject::tr ("Radius")
                        , ViewUtil::spinBox (0.001f, initRadius));
    ViewUtil::connect (this->radiusEdit, [this] (double r) { this->setRadius (float (r)); });

    // connect position edit
    this->positionEdit = this->self->toolOptions ()->add <ViewVectorEdit>
                          ( QObject::tr ("Position")
                          , new ViewVectorEdit (this->movement.position ()));
    QObject::connect (this->positionEdit, &ViewVectorEdit::vectorEdited, [this] 
        (const glm::vec3& p) { this->setPosition (p); });

    // setup mesh
    this->setSubdivision (initSubdivisions);
    this->setRadius      (initRadius);
    this->setPosition    (this->movement.position ());

    this->hover (this->self->mainWindow ()->glWidget ()->cursorPosition ());
  }

  ~Impl () {
    this->fromControlSphere ();
    State::history ().add <ActionNewWingedMesh> ()->run (MeshType::Freeform, this->mesh);
  }

  static QString toolName () {
    return QObject::tr ("New Mesh");
  }

  void setSubdivision (int numSubdiv) {
    if (   numSubdiv >= this->subdivEdit->minimum () 
        && numSubdiv <= this->subdivEdit->maximum ()) 
    {
      this->numSubdivisions = numSubdiv;
      this->mesh = Mesh::icosphere (numSubdiv);
      this->mesh.bufferData  ();
      this->self->mainWindow ()->glWidget ()->update ();
      Config::cache <int> ("/cache/tool/new-freeform-mesh/subdivisions", numSubdiv);
    }
  }

  void setPosition (const glm::vec3& pos) {
    this->movement.position   (pos);
    this->meshGeometry.center (pos);
    this->self->mainWindow ()->glWidget ()->update ();
  }

  void setRadius (float radius) {
    this->meshGeometry.radius (radius);
    this->self->mainWindow ()->glWidget ()->update ();
    Config::cache <float> ("/cache/tool/new-freeform-mesh/radius", radius);
  }

  void setRadiusByMovement (const glm::ivec2& pos) {
    glm::vec3 radiusPoint;
    if (this->movement.onCameraPlane (pos, &radiusPoint)) {
      float d = glm::distance (radiusPoint, this->movement.position ());
      this->setRadius (d);
      this->radiusEdit->setValue (d);
    }
  }

  void setSubdivisionByEvent (int value) {
    this->setSubdivision       (value);
    this->subdivEdit->setValue (value);
  }

  void updateDialog () {
    this->subdivEdit->setValue (this->numSubdivisions);
    this->radiusEdit->setValue (this->meshGeometry.radius ());
    this->positionEdit->vector (this->meshGeometry.center ());
  }

  void fromControlSphere () { 
    this->mesh.scaling  (glm::vec3 (this->meshGeometry.radius ()));
    this->mesh.position (this->meshGeometry.center ());
  }

  void runRender () {
    this->fromControlSphere ();
    this->mesh.render       ();
  }
  
  void hover (const glm::ivec2& pos) {
    this->self->hover (IntersectionUtil::intersects ( State::camera ().ray (pos)
                                                    , this->meshGeometry, nullptr));
  }

  bool runMouseMoveEvent (QMouseEvent* e) {
    glm::ivec2 pos = ViewUtil::toIVec2 (*e);
    if (this->self->isDraged ()) {
      if (this->movement.byMouseEvent (this->self->mainWindow ()->properties ()->movement (), e)) {
        this->setPosition  (this->movement.position ());
        this->updateDialog ();
        return true;
      }
      else if (e->modifiers ().testFlag (Qt::ControlModifier)) {
        glm::vec3 radiusPoint;
        if (this->movement.onCameraPlane (pos, &radiusPoint)) {
          float d = glm::distance (radiusPoint, this->movement.position ());
          this->setRadius    (d);
          this->updateDialog ();
        }
      }
    }
    else {
      this->hover (pos);
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
      if (e->angleDelta ().y () > 0) {
        this->setSubdivision (this->subdivEdit->value () + 1);
      }
      else if (e->angleDelta ().y () < 0) {
        this->setSubdivision (this->subdivEdit->value () - 1);
      }
      this->updateDialog ();
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
