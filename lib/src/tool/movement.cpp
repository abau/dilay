#include <glm/glm.hpp>
#include <QMouseEvent>
#include "tool/movement.hpp"
#include "tool.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "state.hpp"
#include "camera.hpp"
#include "intersection.hpp"
#include "view/properties/movement.hpp"
#include "view/properties/widget.hpp"
#include "view/util.hpp"
#include "view/main-window.hpp"
#include "view/gl-widget.hpp"

struct ToolMovement::Impl {
  Tool*     tool;
  glm::vec3 position;

  Impl (Tool* t) : tool (t) {
    this->byScreenPos ();
  }

  bool intersects (const glm::vec3& normal, const glm::ivec2& p, glm::vec3& i) const {
    const PrimRay   ray   = State::camera ().ray (p);
    const PrimPlane plane (this->position, normal);
          float     t;
    
    if (IntersectionUtil::intersects (ray, plane, &t)) {
      i = ray.pointAt (t);
      return true;
    }
    return false;
  }

  bool moveOnPlane (const glm::vec3& normal, const glm::ivec2& p) {
    return this->intersects (normal, p, this->position);
  }

  bool moveOnPlane (unsigned int normalIndex, const glm::ivec2& p) {
    glm::vec3 normal (0.0f);
    normal[normalIndex] = 1.0f;
    return this->moveOnPlane (normal, p);
  }

  bool moveOnNormal (unsigned int normalIndex, const glm::ivec2& p) {
    glm::vec3 normal    = State::camera ().toEyePoint ();
    normal[normalIndex] = 0.0f;

    glm::vec3 intersection;
    if (this->intersects (glm::normalize (normal), p, intersection)) {
      this->position[normalIndex] = intersection[normalIndex];
      return true;
    }
    return false;
  }

  bool moveOnXY     (const glm::ivec2& p) { return this->moveOnPlane  (2, p); }
  bool moveOnYZ     (const glm::ivec2& p) { return this->moveOnPlane  (0, p); }
  bool moveOnXZ     (const glm::ivec2& p) { return this->moveOnPlane  (1, p); }
  bool moveAlongX   (const glm::ivec2& p) { return this->moveOnNormal (0, p); }
  bool moveAlongY   (const glm::ivec2& p) { return this->moveOnNormal (1, p); }
  bool moveAlongZ   (const glm::ivec2& p) { return this->moveOnNormal (2, p); }

  bool moveOnCamera (const glm::ivec2& p) { 
    return this->moveOnPlane (glm::normalize (State::camera ().toEyePoint ()), p); 
  }

  bool byMouseEvent (QMouseEvent* e) {
    if (e->buttons () == Qt::LeftButton) {
      glm::ivec2 p = ViewUtil::toIVec2 (*e);

      if (e->modifiers ().testFlag (Qt::ShiftModifier)) {
        ViewPropertiesMovement* properties = this->tool->mainWindow ()->properties ()->movement ();
        if      (properties->x ()) { return this->moveAlongX (p); }
        else if (properties->y ()) { return this->moveAlongY (p); }
        else if (properties->z ()) { return this->moveAlongZ (p); }
      }
      else if (e->modifiers ().testFlag (Qt::NoModifier)) {
        return this->byScreenPos (p);
      }
    }
    return false;
  }

  bool byScreenPos (const glm::ivec2& p) {
    ViewPropertiesMovement* properties = this->tool->mainWindow ()->properties ()->movement ();
    if      (properties->xy     ()) { return this->moveOnXY     (p); }
    else if (properties->yz     ()) { return this->moveOnYZ     (p); }
    else if (properties->xz     ()) { return this->moveOnXZ     (p); }
    else if (properties->camera ()) { return this->moveOnCamera (p); }
    else assert (false);
  }

  bool byScreenPos () {
    return this->byScreenPos (this->tool->mainWindow ()->glWidget ()->cursorPosition ());
  }

  bool onCameraPlane (const glm::ivec2& p, glm::vec3* intersection) const {
    return this->intersects (glm::normalize (State::camera ().toEyePoint ()), p, *intersection); 
  }
};

DELEGATE1_BIG6  (ToolMovement, Tool*)
GETTER_CONST    (const glm::vec3&, ToolMovement, position)
SETTER          (const glm::vec3&, ToolMovement, position)
DELEGATE1       (bool            , ToolMovement, byMouseEvent, QMouseEvent*)
DELEGATE        (bool            , ToolMovement, byScreenPos)
DELEGATE2_CONST (bool            , ToolMovement, onCameraPlane, const glm::ivec2&, glm::vec3*)
