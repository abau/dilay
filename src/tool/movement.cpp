#include <glm/glm.hpp>
#include "tool/movement.hpp"
#include "plane.hpp"
#include "ray.hpp"
#include "state.hpp"
#include "camera.hpp"
#include "intersection.hpp"

struct ToolMovement::Impl {
  glm::vec3 position;

  bool moveXZ (const glm::uvec2& mousePos) {
    glm::vec3 normal (0.0f, State::camera ().toEyePoint ().y, 0.0f);

    Plane plane (this->position, glm::normalize (normal));

    const Ray   ray = State::camera ().getRay (mousePos);
          float t;
    
    if (IntersectionUtil::intersects (ray, plane, t)) {
      this->position = ray.pointAt (t);
      return true;
    }
    return false;
  }

  bool moveY (const glm::uvec2& mousePos) {
    glm::vec3 normal   = State::camera ().toEyePoint ();
              normal.y = 0.0f;

    Plane plane (this->position, glm::normalize (normal));

    const Ray   ray = State::camera ().getRay (mousePos);
          float t;
    
    if (IntersectionUtil::intersects (ray, plane, t)) {
      this->position.y = ray.pointAt (t).y;
      return true;
    }
    return false;
  }
};

DELEGATE_BIG6  (ToolMovement)
DELEGATE1      (bool            , ToolMovement, moveXZ, const glm::uvec2&)
DELEGATE1      (bool            , ToolMovement, moveY , const glm::uvec2&)
GETTER_CONST   (const glm::vec3&, ToolMovement, position)
SETTER         (const glm::vec3&, ToolMovement, position)
