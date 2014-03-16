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
#include "render-mode.hpp"
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
    QObject::connect (numSubdivBox, ptr, [this] (int n) { this->setMeshSlot (n); });

    // connect position edit
    this->positionEdit = this->self->toolOptions ()->vectorEdit (QObject::tr ("Position"));
    QObject::connect (this->positionEdit, &ViewVectorEdit::vectorEdited, [this] 
        (const glm::vec3& p) { this->setMeshSlot (p); });

    // setup mesh
    this->setMeshSlot       (initialSubdivisions);
    this->movement.moveXZ   (glm::uvec2 ( this->self->menuEvent ()->x ()
                                        , this->self->menuEvent ()->y ()));
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

  void setMeshSlot (int numSubdivisions) {
    glm::vec3 oldPos = this->mesh.getPosition ();
    this->mesh = Mesh::icosphere (numSubdivisions);
    this->mesh.bufferData  ();
    this->mesh.setPosition (oldPos);
    this->self->mainWindow ()->glWidget ()->update ();
    Config::set <int> ("/cache/tool/new-freeform-mesh/subdivisions", numSubdivisions);
  }

  void setMeshSlot (const glm::vec3& pos) {
    this->movement.position (pos);
    this->mesh.setPosition  (pos);
    this->self->mainWindow  ()->glWidget ()->update ();
  }

  void setMeshByMovement () {
    this->mesh.setPosition     (this->movement.position ());
    this->positionEdit->vector (this->movement.position ());
  }

  void runRender () {
    this->mesh.render ();
  }

  void runMouseMoveEvent (QMouseEvent* e) {
    this->runMousePressEvent (e);
  }

  void runMousePressEvent (QMouseEvent* e) {
    if (e->buttons () == Qt::LeftButton) {
      glm::uvec2 mousePos (e->x (), e->y ());

      if (e->modifiers ().testFlag (Qt::ShiftModifier) ) {
        if (this->movement.moveY (mousePos)) {
          this->setMeshByMovement ();
          this->self->mainWindow ()->glWidget ()->update ();
        }
      }
      else {
        if (this->movement.moveXZ (mousePos)) {
          this->setMeshByMovement ();
          this->self->mainWindow ()->glWidget ()->update ();
        }
      }
    }
  }
};

DELEGATE_TOOL   (ToolNewFreeformMesh)
DELEGATE        (void, ToolNewFreeformMesh, runRender)
DELEGATE1       (void, ToolNewFreeformMesh, runMouseMoveEvent, QMouseEvent*)
DELEGATE1       (void, ToolNewFreeformMesh, runMousePressEvent, QMouseEvent*)
