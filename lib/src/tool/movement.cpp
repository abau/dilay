#include <glm/glm.hpp>
#include <QMouseEvent>
#include "tool/movement.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "state.hpp"
#include "camera.hpp"
#include "intersection.hpp"
#include "dimension.hpp"
#include "view/util.hpp"

struct ToolMovement::Impl {
        Movement  movement;
  const glm::vec3 originalPosition;
        glm::vec3 position;

  Impl (Movement m, const glm::vec3& p) 
    : movement         (m)
    , originalPosition (p)
    , position         (p)
    {}

  Impl (Movement m) 
    : Impl (m, glm::vec3 (0.0f)) 
    {}

  glm::vec3 delta () const {
    return this->position - this->originalPosition;
  }

  bool intersects (const glm::vec3& normal, const glm::ivec2& p, glm::vec3& i) const {
    const PrimRay   ray = State::camera ().ray (p);
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

  bool moveOnPlane (Dimension normalDim, const glm::ivec2& p) {
    glm::vec3 normal (0.0f);
    normal[DimensionUtil::index (normalDim)] = 1.0f;
    return this->moveOnPlane (normal, p);
  }

  bool moveAlong (Dimension axisDim, const glm::ivec2& p) {
    const unsigned int i = DimensionUtil::index (axisDim);
    const glm::vec3    e = State::camera ().toEyePoint ();

    if (glm::dot (DimensionUtil::vector (axisDim), glm::normalize (e)) < 0.9) {
      glm::vec3 normal    = e;
                normal[i] = 0.0f;
      glm::vec3 intersection;

      if (this->intersects (glm::normalize (normal), p, intersection)) {
        this->position[i] = intersection[i];
        return true;
      }
    }
    return false;
  }

  bool moveOnXY     (const glm::ivec2& p) { return this->moveOnPlane (Dimension::Z, p); }
  bool moveOnYZ     (const glm::ivec2& p) { return this->moveOnPlane (Dimension::X, p); }
  bool moveOnXZ     (const glm::ivec2& p) { return this->moveOnPlane (Dimension::Y, p); }
  bool moveAlongX   (const glm::ivec2& p) { return this->moveAlong   (Dimension::X, p); }
  bool moveAlongY   (const glm::ivec2& p) { return this->moveAlong   (Dimension::Y, p); }
  bool moveAlongZ   (const glm::ivec2& p) { return this->moveAlong   (Dimension::Z, p); }

  bool moveOnCameraPlane (const glm::ivec2& p) { 
    return this->moveOnPlane (glm::normalize (State::camera ().toEyePoint ()), p); 
  }

  bool moveOnPrimaryPlane (const glm::ivec2& p) {
    return this->moveOnPlane (State::camera ().primaryDimension (), p);
  }

  bool byMouseEvent (QMouseEvent& e) {
    glm::ivec2 p = ViewUtil::toIVec2 (e);

    if (this->movement == Movement::CameraPlane) {
      return this->moveOnCameraPlane (p);
    }
    else if (this->movement == Movement::PrimaryPlane) {
      return this->moveOnPrimaryPlane (p);
    }
    else {
      if (e.modifiers ().testFlag (Qt::ShiftModifier)) {
        switch (this->movement) {
          case Movement::X: return this->moveOnYZ (p);
          case Movement::Y: return this->moveOnXZ (p);
          case Movement::Z: return this->moveOnXY (p);
          default:          assert (false);
        }
      }
      else {
        switch (this->movement) {
          case Movement::X: return this->moveAlongX (p);
          case Movement::Y: return this->moveAlongY (p);
          case Movement::Z: return this->moveAlongZ (p);
          default:          assert (false);
        }
      }
    }
  }

  bool onCameraPlane (const glm::ivec2& p, glm::vec3& intersection) const {
    return this->intersects (glm::normalize (State::camera ().toEyePoint ()), p, intersection); 
  }
};

DELEGATE2_BIG6        (ToolMovement, Movement, const glm::vec3&)
DELEGATE1_CONSTRUCTOR (ToolMovement, Movement)
GETTER_CONST    (const glm::vec3&, ToolMovement, originalPosition)
DELEGATE_CONST  (glm::vec3       , ToolMovement, delta)
GETTER_CONST    (const glm::vec3&, ToolMovement, position)
SETTER          (const glm::vec3&, ToolMovement, position)
DELEGATE1       (bool            , ToolMovement, byMouseEvent, QMouseEvent&)
DELEGATE2_CONST (bool            , ToolMovement, onCameraPlane, const glm::ivec2&, glm::vec3&)
