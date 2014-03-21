#include <glm/glm.hpp>
#include <QMouseEvent>
#include <QSpinBox>
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

struct ToolNewFreeformMesh::Impl {
  ToolNewFreeformMesh* self;
  Mesh                 mesh;
  ToolMovement         movement;
  ViewVectorEdit*      positionEdit;

  Impl (ToolNewFreeformMesh* s) : self (s) {
    int initialSubdivisions = Config::get <int> ("/cache/tool/new-freeform-mesh/subdivisions", 2);

    // connect spin box
    QSpinBox* numSubdivBox = this->self->toolOptions ()
                                       ->spinBox ( QObject::tr ("Subdivisions")
                                                 , 1, initialSubdivisions, 5);
    void (QSpinBox::* ptr)(int) = &QSpinBox::valueChanged;
    QObject::connect (numSubdivBox, ptr, [this] (int n) { this->setMeshByInput (n); });

    // connect position edit
    this->positionEdit = this->self->toolOptions ()->vectorEdit (QObject::tr ("Position"));
    QObject::connect (this->positionEdit, &ViewVectorEdit::vectorEdited, [this] 
        (const glm::vec3& p) { this->setMeshByInput (p); });

    // setup mesh
    this->setMeshByInput    (initialSubdivisions);
    this->movement.moveXZ   (this->self->menuEvent ()->pos ());
    this->setMeshByMovement ();

    this->self->mainWindow ()->glWidget ()->setCursor (QCursor (Qt::SizeAllCursor));
  }

  ~Impl () {
    State::history ().add <ActionNewWingedMesh> ()->run (MeshType::FreeForm, this->mesh);
    this->self->mainWindow ()->glWidget ()->unsetCursor ();
  }

  static QString toolName () {
    return QObject::tr ("New Freeform Mesh");
  }

  void setMeshByInput (int numSubdivisions) {
    glm::vec3 oldPos = this->mesh.position ();
    this->mesh = Mesh::icosphere (numSubdivisions);
    this->mesh.bufferData  ();
    this->mesh.position    (oldPos);
    this->self->mainWindow ()->glWidget ()->update ();
    Config::set <int> ("/cache/tool/new-freeform-mesh/subdivisions", numSubdivisions);
  }

  void setMeshByInput (const glm::vec3& pos) {
    this->movement.position (pos);
    this->mesh.position     (pos);
    this->self->mainWindow  ()->glWidget ()->update ();
  }

  void setMeshByMovement () {
    this->mesh.position        (this->movement.position ());
    this->positionEdit->vector (this->movement.position ());
  }

  void runRender () {
    this->mesh.render ();
  }

  bool runMouseMoveEvent (QMouseEvent* e) {
    return this->runMousePressEvent (e);
  }

  bool runMousePressEvent (QMouseEvent* e) {
    if (this->movement.byMouseEvent (e)) {
      this->setMeshByMovement ();
      return true;
    }
    return false;
  }
};

DELEGATE_TOOL   (ToolNewFreeformMesh)
DELEGATE        (void, ToolNewFreeformMesh, runRender)
DELEGATE1       (bool, ToolNewFreeformMesh, runMouseMoveEvent, QMouseEvent*)
DELEGATE1       (bool, ToolNewFreeformMesh, runMousePressEvent, QMouseEvent*)
