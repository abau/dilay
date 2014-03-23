#include <glm/glm.hpp>
#include <QMouseEvent>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include "tool/new-freeform-mesh.hpp"
#include "view/main-window.hpp"
#include "view/gl-widget.hpp"
#include "action/new-winged-mesh.hpp"
#include "mesh-type.hpp"
#include "mesh.hpp"
#include "state.hpp"
#include "history.hpp"
#include "tool/movement.hpp"
#include "view/tool-options.hpp"
#include "view/vector-edit.hpp"
#include "config.hpp"
#include "view/util.hpp"

struct ToolNewFreeformMesh::Impl {
  ToolNewFreeformMesh* self;
  Mesh                 mesh;
  ToolMovement         movement;
  ViewVectorEdit*      positionEdit;

  Impl (ToolNewFreeformMesh* s) : self (s) {
    int   initSubdivisions = Config::get <int>   ("/cache/tool/new-freeform-mesh/subdivisions", 2);
    float initRadius       = Config::get <float> ("/cache/tool/new-freeform-mesh/radius", 1.0f);

    // connect subdivisions spin box
    QSpinBox* numSubdivBox = this->self->toolOptions ()->add <QSpinBox> 
                              ( QObject::tr ("Subdivisions")
                              , ViewUtil::spinBox (1, initSubdivisions, 5));
    void (QSpinBox::* iPtr)(int) = &QSpinBox::valueChanged;
    QObject::connect (numSubdivBox, iPtr, [this] (int n) { this->setMeshByInput (n); });

    // connect radius spin box
    QDoubleSpinBox* radiusBox = this->self->toolOptions ()->add <QDoubleSpinBox>
                                  ( QObject::tr ("Radius")
                                  , ViewUtil::spinBox (0.0001f, initRadius, 0.1f, 100.f));
    void (QDoubleSpinBox::* dPtr)(double) = &QDoubleSpinBox::valueChanged;
    QObject::connect (radiusBox, dPtr, [this] (double r) { this->setMeshByInput (float (r)); });

    // connect position edit
    this->positionEdit = this->self->toolOptions ()->add <ViewVectorEdit>
                          (QObject::tr ("Position"), new ViewVectorEdit);
    QObject::connect (this->positionEdit, &ViewVectorEdit::vectorEdited, [this] 
        (const glm::vec3& p) { this->setMeshByInput (p); });

    // setup mesh
    this->setMeshByInput    (initSubdivisions);
    this->movement.moveXZ   (this->self->menuEvent ()->pos ());
    this->setMeshByMovement ();

    this->self->mainWindow ()->glWidget ()->setCursor (QCursor (Qt::SizeAllCursor));
  }

  ~Impl () {
    State::history ().add <ActionNewWingedMesh> ()->run (MeshType::Freeform, this->mesh);
    this->self->mainWindow ()->glWidget ()->unsetCursor ();
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
    this->self->mainWindow  ()->glWidget ()->update ();
  }

  void setMeshByInput (float radius) {
    this->mesh.scaling     (glm::vec3 (radius));
    this->self->mainWindow ()->glWidget ()->update ();
  }

  void setMeshByMovement () {
    this->mesh.position        (this->movement.position ());
    this->positionEdit->vector (this->movement.position ());
  }

  void runRender () {
    this->mesh.render ();
  }

  bool runMouseMoveEvent (QMouseEvent* e) {
    if (e->buttons ().testFlag (Qt::LeftButton)) {
      if (this->movement.byMouseEvent (e)) {
        this->setMeshByMovement ();
        return true;
      }
    }
    return false;
  }
};

DELEGATE_TOOL   (ToolNewFreeformMesh)
DELEGATE        (void, ToolNewFreeformMesh, runRender)
DELEGATE1       (bool, ToolNewFreeformMesh, runMouseMoveEvent, QMouseEvent*)
