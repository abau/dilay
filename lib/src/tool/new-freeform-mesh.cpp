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
  ConfigProxy          config;

  Impl (ToolNewFreeformMesh* s) 
    : self     (s) 
    , movement (s)
    , config   ("/cache/tool/new-freeform-mesh/")
  {
    int   initSubdivisions = this->config.get <int>   ("subdivisions", 2);
    float initRadius       = this->config.get <float> ("radius", 1.0f);

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
    this->fromMeshGeometry ();
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
      this->mesh.bufferData        ();
      this->self->updateGlWidget   ();
      this->config.cache <int>     ("subdivisions", numSubdiv);
    }
  }

  void setPosition (const glm::vec3& pos) {
    this->movement.position    (pos);
    this->meshGeometry.center  (pos);
    this->self->updateGlWidget ();
  }

  void setRadius (float radius) {
    this->meshGeometry.radius  (radius);
    this->self->updateGlWidget ();
    this->config.cache <float> ("radius", radius);
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

  void fromMeshGeometry () { 
    this->mesh.scaling  (glm::vec3 (this->meshGeometry.radius ()));
    this->mesh.position (this->meshGeometry.center ());
  }

  void runRender () {
    this->fromMeshGeometry ();
    this->mesh.render      ();
  }
  
  void hover (const glm::ivec2& pos) {
    this->self->hover (IntersectionUtil::intersects ( State::camera ().ray (pos)
                                                    , this->meshGeometry, nullptr));
  }

  bool runMouseMoveEvent (QMouseEvent* e) {
    glm::ivec2 pos = ViewUtil::toIVec2 (*e);
    if (this->self->isDraged ()) {
      if (this->movement.byMouseEvent (e)) {
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

DELEGATE_BIG3_BASE ( ToolNewFreeformMesh, (ViewMainWindow* w, QContextMenuEvent* e)
                   , (this), Tool, (w, e, toolName ()))
DELEGATE_STATIC (QString, ToolNewFreeformMesh, toolName)
DELEGATE        (void   , ToolNewFreeformMesh, runRender)
DELEGATE1       (bool   , ToolNewFreeformMesh, runMouseMoveEvent, QMouseEvent*)
DELEGATE1       (bool   , ToolNewFreeformMesh, runMousePressEvent, QMouseEvent*)
DELEGATE1       (bool   , ToolNewFreeformMesh, runMouseReleaseEvent, QMouseEvent*)
DELEGATE1       (bool, ToolNewFreeformMesh, runWheelEvent, QWheelEvent*)
