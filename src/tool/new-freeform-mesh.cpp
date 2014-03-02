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
#include "rendermode.hpp"
#include "tool/movement.hpp"
#include "tool/options.hpp"

struct ToolNewFreeformMesh::Impl {
  ToolNewFreeformMesh* self;
  Mesh                 mesh;
  ToolMovement         movement;

  Impl (ToolNewFreeformMesh* s) 
    : self    (s) 
  {}

  void runInitialize (QContextMenuEvent* e) {
    this->setMesh (2);
    this->movement.moveXZ  (glm::uvec2 (e->x (), e->y ()));
    this->mesh.setPosition (this->movement.position ());

    QSpinBox* numSubdivBox = this->self->toolOptions ()->spinBox ("subdivisions", 1, 2, 5);
    void (QSpinBox::* ptr)(int) = &QSpinBox::valueChanged;
    QObject::connect (numSubdivBox, ptr, [this] (int n) { this->setMesh (n); });
  }

  void runApply () {
    State::history ().add <ActionNewMesh> ()->newMesh (MeshType::FreeForm, this->mesh);
  }

  void setMesh (int numSubdivisions) {
    glm::vec3 oldPos = this->mesh.getPosition ();
    this->mesh = Mesh::icosphere (numSubdivisions);
    this->mesh.renderMode  (RenderMode::Flat);
    this->mesh.bufferData  ();
    this->mesh.setPosition (oldPos);

    this->self->mainWindow ()->glWidget ()->update ();
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
          this->mesh.setPosition (this->movement.position ());
          this->self->mainWindow ()->glWidget ()->update ();
        }
      }
      else {
        if (this->movement.moveXZ (mousePos)) {
          this->mesh.setPosition (this->movement.position ());
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
