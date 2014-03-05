#include <memory>
#include <glm/glm.hpp>
#include <QMouseEvent>
#include <QSpinBox>
#include <QPushButton>
#include "tool/new-freeform-mesh.hpp"
#include "view/main-window.hpp"
#include "view/gl-widget.hpp"
#include "action/new-mesh.hpp"
#include "mesh-type.hpp"
#include "mesh.hpp"
#include "state.hpp"
#include "history.hpp"
#include "render-mode.hpp"
#include "tool/movement.hpp"
#include "view/tool-options.hpp"
#include "view/vector-edit.hpp"

struct ToolNewFreeformMesh::Impl {
  ToolNewFreeformMesh* self;
  Mesh                 mesh;
  ToolMovement         movement;
  ViewVectorEdit*      positionEdit;

  Impl (ToolNewFreeformMesh* s) 
    : self    (s) 
  {
    this->self->toolName (QObject::tr ("New Freeform Mesh"));
  }

  void runInitialize (QContextMenuEvent* e) {
    // connect spin box
    QSpinBox* numSubdivBox = this->self->toolOptions ()
                                       ->spinBox (QObject::tr ("Subdivisions"), 1, 2, 5);
    void (QSpinBox::* ptr)(int) = &QSpinBox::valueChanged;
    QObject::connect (numSubdivBox, ptr, [this] (int n) { this->setMeshSlot (n); });

    // connect position edit
    this->positionEdit = this->self->toolOptions ()->vectorEdit (QObject::tr ("Position"));
    QObject::connect (this->positionEdit, &ViewVectorEdit::vectorEdited, [this] 
        (const glm::vec3& p) { this->setMeshSlot (p); });

    // setup mesh
    this->setMeshSlot       (2);
    this->movement.moveXZ   (glm::uvec2 (e->x (), e->y ()));
    this->setMeshByMovement ();

    this->self->mainWindow ()->glWidget ()->setCursor (QCursor (Qt::SizeAllCursor));
  }

  void runApply () {
    State::history ().add <ActionNewMesh> ()->newMesh (MeshType::FreeForm, this->mesh);
  }

  void runClose () {
    this->self->mainWindow ()->glWidget ()->unsetCursor ();
  }

  void setMeshSlot (int numSubdivisions) {
    glm::vec3 oldPos = this->mesh.getPosition ();
    this->mesh = Mesh::icosphere (numSubdivisions);
    this->mesh.bufferData  ();
    this->mesh.setPosition (oldPos);
    this->self->mainWindow ()->glWidget ()->update ();
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

DELEGATE_BIG3_SELF (ToolNewFreeformMesh)
DELEGATE1 (void, ToolNewFreeformMesh, runInitialize, QContextMenuEvent*)
DELEGATE  (void, ToolNewFreeformMesh, runRender)
DELEGATE1 (void, ToolNewFreeformMesh, runMouseMoveEvent, QMouseEvent*)
DELEGATE1 (void, ToolNewFreeformMesh, runMousePressEvent, QMouseEvent*)
DELEGATE  (void, ToolNewFreeformMesh, runApply)
DELEGATE  (void, ToolNewFreeformMesh, runClose)
