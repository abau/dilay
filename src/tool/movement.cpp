#include <glm/glm.hpp>
#include <QMouseEvent>
#include "tool/movement.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "state.hpp"
#include "camera.hpp"
#include "intersection.hpp"
#include "view/properties/movement.hpp"

struct ToolMovement::Impl {
  glm::vec3 position;

  bool moveOnPlane (unsigned int normalIndex, const glm::uvec2& mousePos) {
    glm::vec3 normal (0.0f);
    normal[normalIndex] = 1.0f;

    const PrimRay   ray   = State::camera ().getRay (mousePos);
    const PrimPlane plane (this->position, normal);
          float     t;
    
    if (IntersectionUtil::intersects (ray, plane, t)) {
      this->position = ray.pointAt (t);
      return true;
    }
    return false;
  }

  bool moveOnNormal (unsigned int normalIndex, const glm::uvec2& mousePos) {
    glm::vec3 normal    = State::camera ().toEyePoint ();
    normal[normalIndex] = 0.0f;

    const PrimRay   ray   = State::camera ().getRay (mousePos);
    const PrimPlane plane (this->position, glm::normalize (normal));
          float     t;
    
    if (IntersectionUtil::intersects (ray, plane, t)) {
      this->position[normalIndex] = ray.pointAt (t)[normalIndex];
      return true;
    }
    return false;
  }

  bool moveXY (const glm::uvec2& p) { return this->moveOnPlane  (2, p); }
  bool moveYZ (const glm::uvec2& p) { return this->moveOnPlane  (0, p); }
  bool moveXZ (const glm::uvec2& p) { return this->moveOnPlane  (1, p); }
  bool moveX  (const glm::uvec2& p) { return this->moveOnNormal (0, p); }
  bool moveY  (const glm::uvec2& p) { return this->moveOnNormal (1, p); }
  bool moveZ  (const glm::uvec2& p) { return this->moveOnNormal (2, p); }

  bool byMouseEvent (ViewPropertiesMovement* properties, QMouseEvent* e) {
    if (e->buttons () == Qt::LeftButton) {
      glm::uvec2 mousePos (e->x (), e->y ());

      if (e->modifiers ().testFlag (Qt::ShiftModifier) ) {
        if      (properties->x ()) { return this->moveX (mousePos); }
        else if (properties->y ()) { return this->moveY (mousePos); }
        else if (properties->z ()) { return this->moveZ (mousePos); }
        else assert (false);
      }
      else {
        return this->byScreenPos (properties, mousePos);
      }
    }
    return false;
  }

  bool byScreenPos (ViewPropertiesMovement* properties, const glm::uvec2& p) {
    if      (properties->xy ()) { return this->moveXY (p); }
    else if (properties->yz ()) { return this->moveYZ (p); }
    else if (properties->xz ()) { return this->moveXZ (p); }
    else assert (false);
  }
};

DELEGATE_BIG6  (ToolMovement)
GETTER_CONST   (const glm::vec3&, ToolMovement, position)
SETTER         (const glm::vec3&, ToolMovement, position)
DELEGATE2      (bool            , ToolMovement, byMouseEvent, ViewPropertiesMovement*, QMouseEvent*)
DELEGATE2      (bool            , ToolMovement, byScreenPos , ViewPropertiesMovement*, const glm::uvec2&)
