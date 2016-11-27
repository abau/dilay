/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QAbstractButton>
#include <QButtonGroup>
#include "../../util.hpp"
#include "camera.hpp"
#include "dimension.hpp"
#include "intersection.hpp"
#include "kvstore.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "tool/util/movement.hpp"
#include "view/pointing-event.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

namespace {
  int constraintToInt (MovementConstraint c) {
    switch (c) {
      case MovementConstraint::XAxis:        return 0;
      case MovementConstraint::YAxis:        return 1;
      case MovementConstraint::ZAxis:        return 2;
      case MovementConstraint::XYPlane:      return 3;
      case MovementConstraint::XZPlane:      return 4;
      case MovementConstraint::YZPlane:      return 5;
      case MovementConstraint::CameraPlane:  return 6;
      case MovementConstraint::PrimaryPlane: return 7;
      default:
        DILAY_IMPOSSIBLE
    }
  }

  MovementConstraint constraintFromInt (int i) {
    switch (i) {
      case 0: return MovementConstraint::XAxis;
      case 1: return MovementConstraint::YAxis;
      case 2: return MovementConstraint::ZAxis;
      case 3: return MovementConstraint::XYPlane;
      case 4: return MovementConstraint::XZPlane;
      case 5: return MovementConstraint::YZPlane;
      case 6: return MovementConstraint::CameraPlane;
      case 7: return MovementConstraint::PrimaryPlane;
      default:
        DILAY_IMPOSSIBLE
    }
  }
}

struct ToolUtilMovement::Impl {
  const Camera&      camera;
  MovementConstraint constraint;
  glm::vec3          previousPosition;
  glm::vec3          position;

  Impl (const Camera& cam, MovementConstraint c) 
    : camera           (cam)
    , constraint       (c)
    , previousPosition (glm::vec3 (0.0f))
    , position         (glm::vec3 (0.0f))
  {}

  glm::vec3 delta () const {
    return this->position - this->previousPosition;
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
    glm::vec3 i;
    if (this->intersects (normal, p, i)) {
      this->previousPosition = this->position;
      this->position         = i;
      return true;
    }
    else {
      return false;
    }
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

      if (this->intersects (normal, p, intersection)) {
        this->previousPosition = this->position;
        this->position[i]      = intersection[i];
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
        return this->moveOnPlane (this->camera.toEyePoint (), p); 

      case MovementConstraint::PrimaryPlane: 
        return modify ? this->moveAlong   (this->camera.primaryDimension (), p)
                      : this->moveOnPlane (this->camera.primaryDimension (), p);

      default:
        DILAY_IMPOSSIBLE
    }
  }

  bool move (const ViewPointingEvent& e, bool considerShift) {
    return this->move (e.ivec2 (), considerShift && e.modifiers () == Qt::ShiftModifier);
  }

  void resetPosition (const glm::vec3& p) {
    this->previousPosition = p;
    this->position         = p;
  }

  void addProperties (ViewTwoColumnGrid& properties, const std::function <void ()>& onClick) {
    QButtonGroup& constraintEdit = *new QButtonGroup;
    properties.add ( constraintEdit , { QObject::tr ("X-axis")
                                      , QObject::tr ("Y-axis")
                                      , QObject::tr ("Z-axis")
                                      , QObject::tr ("XY-plane")
                                      , QObject::tr ("XZ-plane")
                                      , QObject::tr ("YZ-plane")
                                      , QObject::tr ("Camera-plane")
                                      , QObject::tr ("Primary plane") 
                                      } );
    ViewUtil::connect (constraintEdit, [this, onClick] (int id) {
      this->constraint = constraintFromInt (id);
      onClick ();
    });
    constraintEdit.button (constraintToInt (this->constraint))->click ();
  }
};


template <>
void KVStore :: set (const std::string& path, const MovementConstraint& c) {
  this->set (path, constraintToInt (c));
}
  
template <>
MovementConstraint KVStore :: getFrom (const std::string& path, const MovementConstraint& c) const {
  return constraintFromInt (this->get (path, constraintToInt (c)));
}

DELEGATE2_BIG4COPY (ToolUtilMovement, const Camera&, MovementConstraint)
GETTER_CONST    (MovementConstraint, ToolUtilMovement, constraint)
SETTER          (MovementConstraint, ToolUtilMovement, constraint)
DELEGATE_CONST  (glm::vec3         , ToolUtilMovement, delta)
GETTER_CONST    (const glm::vec3&  , ToolUtilMovement, position)
SETTER          (const glm::vec3&  , ToolUtilMovement, position)
DELEGATE2       (bool              , ToolUtilMovement, move, const ViewPointingEvent&, bool)
DELEGATE1       (void              , ToolUtilMovement, resetPosition, const glm::vec3&)
DELEGATE2       (void              , ToolUtilMovement, addProperties, ViewTwoColumnGrid&, const std::function <void ()>&)
