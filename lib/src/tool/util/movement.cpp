#include <QMouseEvent>
#include <glm/glm.hpp>
#include "camera.hpp"
#include "dimension.hpp"
#include "intersection.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "tool/util/movement.hpp"
#include "view/util.hpp"

struct ToolUtilMovement::Impl {
  const Camera&      camera;
  MovementConstraint constraint;
  const glm::vec3    originalPosition;
  glm::vec3          position;

  Impl (const Camera& cam, const glm::vec3& o, MovementConstraint c) 
    : camera           (cam)
    , constraint       (c)
    , originalPosition (o)
    , position         (o)
  {}

  glm::vec3 delta () const {
    return this->position - this->originalPosition;
  }

  void delta (const glm::vec3& d) {
    this->position = this->originalPosition + d;
  }

  bool intersects (const glm::vec3& normal, const glm::ivec2& p, glm::vec3& i) const {
    const PrimRay   ray = this->camera.ray (p);
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
    const glm::vec3    e = this->camera.toEyePoint ();

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

  bool move (const glm::ivec2& p, bool modify) {
    switch (this->constraint) {
      case MovementConstraint::XAxis: 
        return modify ? this->moveOnPlane (Dimension::X, p)
                      : this->moveAlong   (Dimension::X, p);
      case MovementConstraint::YAxis: 
        return modify ? this->moveOnPlane (Dimension::Y, p)
                      : this->moveAlong   (Dimension::Y, p);
      case MovementConstraint::ZAxis: 
        return modify ? this->moveOnPlane (Dimension::Z, p)
                      : this->moveAlong   (Dimension::Z, p);
      case MovementConstraint::XYPlane: 
        return modify ? this->moveAlong   (Dimension::Z, p)
                      : this->moveOnPlane (Dimension::Z, p);
      case MovementConstraint::XZPlane: 
        return modify ? this->moveAlong   (Dimension::Y, p)
                      : this->moveOnPlane (Dimension::Y, p);
      case MovementConstraint::YZPlane: 
        return modify ? this->moveAlong   (Dimension::X, p)
                      : this->moveOnPlane (Dimension::X, p);
      case MovementConstraint::CameraPlane: 
        return this->moveOnPlane (glm::normalize (this->camera.toEyePoint ()), p); 
      case MovementConstraint::PrimaryPlane: 
        return modify ? this->moveAlong   (this->camera.primaryDimension (), p)
                      : this->moveOnPlane (this->camera.primaryDimension (), p);
    }
    std::abort ();
  }

  bool byMouseEvent (QMouseEvent& e) {
    return this->move (ViewUtil::toIVec2 (e), e.modifiers () == Qt::ShiftModifier);
  }
};

DELEGATE3_BIG4COPY (ToolUtilMovement, const Camera&, const glm::vec3&, MovementConstraint)
GETTER_CONST    (MovementConstraint, ToolUtilMovement, constraint)
SETTER          (MovementConstraint, ToolUtilMovement, constraint)
GETTER_CONST    (const glm::vec3&  , ToolUtilMovement, originalPosition)
DELEGATE_CONST  (glm::vec3         , ToolUtilMovement, delta)
DELEGATE1       (void              , ToolUtilMovement, delta, const glm::vec3&)
GETTER_CONST    (const glm::vec3&  , ToolUtilMovement, position)
SETTER          (const glm::vec3&  , ToolUtilMovement, position)
DELEGATE1       (bool              , ToolUtilMovement, byMouseEvent, QMouseEvent&)
