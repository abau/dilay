#include <memory>
#include <glm/glm.hpp>
#include <QMouseEvent>
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

struct ToolNewFreeformMesh::Impl {
  ToolNewFreeformMesh* self;
  Mesh                 mesh;
  Plane                plane;

  Impl (ToolNewFreeformMesh* s) 
    : self  (s) 
    , plane (glm::vec3 (0.0f), glm::vec3 (0.0f, 1.0f, 0.0f))
  {}

  void runInitialize (ViewMainWindow*, QContextMenuEvent* e) {
    this->mesh = Mesh::icosphere (2);
    this->setMeshPosition (glm::uvec2 (e->x (), e->y ()));
    this->mesh.bufferData ();

    //w->bottomToolbar ()->
  }

  void runRender () {
    this->mesh.render ();
  }

  bool setMeshPosition (const glm::uvec2& cameraPos) {
    const Ray   r = State::camera ().getRay (cameraPos);
          float t;
    
    if (IntersectionUtil::intersects (r, this->plane, t)) {
      this->mesh.setPosition (r.pointAt (t));
      return true;
    }
    return false;
  }

  ToolResponse runMouseMoveEvent (QMouseEvent* e) {
    if (this->setMeshPosition (glm::uvec2 (e->x (), e->y ())))
      return ToolResponse::Redraw;
    else
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
