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
    : self   (s) 
    , plane (glm::vec3 (0.0f), glm::vec3 (0.0f, 1.0f, 0.0f))
  {}

  void runInitialize (ViewMainWindow*) {
    this->mesh = Mesh::icosphere (2);
    this->mesh.bufferData ();

    //w->bottomToolbar ()->
  }

  void runRender () {
    this->mesh.render ();
  }

  ToolResponse runMouseMoveEvent (QMouseEvent* e) {
    const Ray   r = State::camera ().getRay (glm::uvec2 (e->x (), e->y ()));
          float t;
    
    if (IntersectionUtil::intersects (r, this->plane, t)) {
      this->mesh.setPosition (r.pointAt (t));
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
DELEGATE1 (void        , ToolNewFreeformMesh, runInitialize, ViewMainWindow*)
DELEGATE  (void        , ToolNewFreeformMesh, runRender)
DELEGATE1 (ToolResponse, ToolNewFreeformMesh, runMouseMoveEvent, QMouseEvent*)
DELEGATE1 (ToolResponse, ToolNewFreeformMesh, runMousePressEvent, QMouseEvent*)
