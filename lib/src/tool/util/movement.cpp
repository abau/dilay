#include <QMouseEvent>
#include <glm/glm.hpp>
#include "camera.hpp"
#include "dimension.hpp"
#include "intersection.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "state.hpp"
#include "tool/util/movement.hpp"
#include "view/util.hpp"

struct ToolUtilMovement::Impl {
  MovementConstraint constraint;
  glm::vec3          originalPosition;
  glm::vec3          position;

  Impl (MovementConstraint c, const glm::ivec2& pos) 
    : constraint       (c)
    , originalPosition (State::camera ().gazePoint ())
    , position         (originalPosition)
  {
    this->move (pos, false);
    this->originalPosition = this->position;
  }

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

  bool move (const glm::ivec2& p, bool reverse) {
    switch (this->constraint) {
      case MovementConstraint::XAxis: 
        return reverse ? this->moveOnPlane (Dimension::X, p)
                       : this->moveAlong   (Dimension::X, p);
      case MovementConstraint::YAxis: 
        return reverse ? this->moveOnPlane (Dimension::Y, p)
                       : this->moveAlong   (Dimension::Y, p);
      case MovementConstraint::ZAxis: 
        return reverse ? this->moveOnPlane (Dimension::Z, p)
                       : this->moveAlong   (Dimension::Z, p);
      case MovementConstraint::XYPlane: 
        return reverse ? this->moveAlong   (Dimension::Z, p)
                       : this->moveOnPlane (Dimension::Z, p);
      case MovementConstraint::XZPlane: 
        return reverse ? this->moveAlong   (Dimension::Y, p)
                       : this->moveOnPlane (Dimension::Y, p);
      case MovementConstraint::YZPlane: 
        return reverse ? this->moveAlong   (Dimension::X, p)
                       : this->moveOnPlane (Dimension::X, p);
      case MovementConstraint::CameraPlane: 
        return this->moveOnPlane (glm::normalize (State::camera ().toEyePoint ()), p); 
      case MovementConstraint::PrimaryPlane: 
        return this->moveOnPlane (State::camera ().primaryDimension (), p);
    }
    std::abort ();
  }

  bool byMouseEvent (QMouseEvent& e) {
    return this->move ( ViewUtil::toIVec2 (e)
                      , e.modifiers ().testFlag (Qt::ShiftModifier));
  }

  bool onCameraPlane (const glm::ivec2& p, glm::vec3& intersection) const {
    return this->intersects (glm::normalize (State::camera ().toEyePoint ()), p, intersection); 
  }
};

DELEGATE2_BIG6  (ToolUtilMovement, MovementConstraint, const glm::ivec2&)
GETTER_CONST    (MovementConstraint, ToolUtilMovement, constraint)
SETTER          (MovementConstraint, ToolUtilMovement, constraint)
GETTER_CONST    (const glm::vec3&  , ToolUtilMovement, originalPosition)
DELEGATE_CONST  (glm::vec3         , ToolUtilMovement, delta)
GETTER_CONST    (const glm::vec3&  , ToolUtilMovement, position)
SETTER          (const glm::vec3&  , ToolUtilMovement, position)
DELEGATE1       (bool              , ToolUtilMovement, byMouseEvent, QMouseEvent&)
DELEGATE2_CONST (bool              , ToolUtilMovement, onCameraPlane, const glm::ivec2&, glm::vec3&)
