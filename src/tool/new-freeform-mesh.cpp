#include <memory>
#include <glm/glm.hpp>
#include <QMouseEvent>
#include <QSpinBox>
#include "tool/new-freeform-mesh.hpp"
#include "view/main-window.hpp"
#include "action/new-mesh.hpp"
#include "mesh-type.hpp"
#include "mesh.hpp"
#include "plane.hpp"
#include "ray.hpp"
#include "state.hpp"
#include "camera.hpp"
#include "intersection.hpp"
#include "history.hpp"
#include "rendermode.hpp"
#include "view/bottom-toolbar.hpp"

struct ToolNewFreeformMesh::Impl {
  ToolNewFreeformMesh* self;
  Mesh                 mesh;
  Plane                xzPlane;

  Impl (ToolNewFreeformMesh* s) 
    : self    (s) 
    , xzPlane (glm::vec3 (0.0f), glm::vec3 (0.0f, 1.0f, 0.0f))
  {}

  void runInitialize (ViewMainWindow* w, QContextMenuEvent* e) {
    this->setMesh (2);
    this->setMeshXZPosition (glm::uvec2 (e->x (), e->y ()));

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

  bool setMeshXZPosition (const glm::uvec2& mousePos) {
    const Ray   ray = State::camera ().getRay (mousePos);
          float t;
    
    if (IntersectionUtil::intersects (ray, this->xzPlane, t)) {
      this->mesh.setPosition (ray.pointAt (t));
      return true;
    }
    return false;
  }

  bool setMeshYPosition (const glm::uvec2& mousePos) {
    glm::vec3 meshPos = this->mesh.getPosition ();
    glm::vec3 gaze    = State::camera ().toEyePoint ();
              gaze.y  = 0.0f;

    Plane plane (meshPos, glm::normalize (gaze));

    const Ray   ray = State::camera ().getRay (mousePos);
          float t;
    
    if (IntersectionUtil::intersects (ray, plane, t)) {
      meshPos.y = ray.pointAt (t).y;
      this->mesh.setPosition (meshPos);
      this->xzPlane.point    (meshPos);
      return true;
    }
    return false;
  }

  ToolResponse runMouseMoveEvent (QMouseEvent* e) {
    glm::uvec2 mousePos (e->x (), e->y ());

    if (e->modifiers ().testFlag (Qt::ShiftModifier) ) {
      if (this->setMeshYPosition (mousePos))
        return ToolResponse::Redraw;
    }
    else {
      if (this->setMeshXZPosition (mousePos))
        return ToolResponse::Redraw;
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
