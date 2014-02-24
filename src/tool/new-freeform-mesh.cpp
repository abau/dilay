#include <memory>
#include <glm/glm.hpp>
#include <QMouseEvent>
#include <QSpinBox>
#include "tool/new-freeform-mesh.hpp"
#include "view/main-window.hpp"
#include "action/new-mesh.hpp"
#include "mesh-type.hpp"
#include "mesh.hpp"
#include "state.hpp"
#include "history.hpp"
#include "rendermode.hpp"
#include "view/bottom-toolbar.hpp"
#include "tool/movement.hpp"

struct ToolNewFreeformMesh::Impl {
  ToolNewFreeformMesh* self;
  Mesh                 mesh;
  ToolMovement         movement;

  Impl (ToolNewFreeformMesh* s) 
    : self    (s) 
  {}

  void runInitialize (ViewMainWindow* w, QContextMenuEvent* e) {
    this->setMesh (2);
    this->movement.moveXZ  (glm::uvec2 (e->x (), e->y ()));
    this->mesh.setPosition (this->movement.position ());

    QSpinBox* numSubdivBox = w->bottomToolbar ()->addSpinBox ("subdivisions", 1, 2, 5);
    void (QSpinBox::* ptr)(int) = &QSpinBox::valueChanged;
    QObject::connect (numSubdivBox, ptr, [this] (int n) { this->setMesh (n); });
  }

  void setMesh (int numSubdivisions) {
    glm::vec3 oldPos = this->mesh.getPosition ();
    this->mesh = Mesh::icosphere (numSubdivisions);
    this->mesh.renderMode  (RenderMode::Flat);
    this->mesh.bufferData  ();
    this->mesh.setPosition (oldPos);
  }

  void runRender () {
    this->mesh.render ();
  }

  ToolResponse runMouseMoveEvent (QMouseEvent* e) {
    glm::uvec2 mousePos (e->x (), e->y ());

    if (e->modifiers ().testFlag (Qt::ShiftModifier) ) {
      if (this->movement.moveY (mousePos)) {
        this->mesh.setPosition (this->movement.position ());
        return ToolResponse::Redraw;
      }
    }
    else {
      if (this->movement.moveXZ (mousePos)) {
        this->mesh.setPosition (this->movement.position ());
        return ToolResponse::Redraw;
      }
    }
    return ToolResponse::None;
  }

  ToolResponse runMousePressEvent (QMouseEvent* e) { 
    if (e->buttons () == Qt::LeftButton) {
      State::history ().add <ActionNewMesh> ()->newMesh (MeshType::FreeForm, this->mesh);
      return ToolResponse::Terminate;
    }
    return ToolResponse::None; 
  }
};

DELEGATE_BIG3_SELF (ToolNewFreeformMesh)
DELEGATE2 (void        , ToolNewFreeformMesh, runInitialize, ViewMainWindow*, QContextMenuEvent*)
DELEGATE  (void        , ToolNewFreeformMesh, runRender)
DELEGATE1 (ToolResponse, ToolNewFreeformMesh, runMouseMoveEvent, QMouseEvent*)
DELEGATE1 (ToolResponse, ToolNewFreeformMesh, runMousePressEvent, QMouseEvent*)
