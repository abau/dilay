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
#include "variant.hpp"
#include "view/pointing-event.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

namespace
{
  int constraintToInt (MovementFixedConstraint c)
  {
    switch (c)
    {
      case MovementFixedConstraint::XAxis:
        return 0;
      case MovementFixedConstraint::YAxis:
        return 1;
      case MovementFixedConstraint::ZAxis:
        return 2;
      case MovementFixedConstraint::XYPlane:
        return 3;
      case MovementFixedConstraint::XZPlane:
        return 4;
      case MovementFixedConstraint::YZPlane:
        return 5;
      case MovementFixedConstraint::CameraPlane:
        return 6;
      case MovementFixedConstraint::PrimaryPlane:
        return 7;
      default:
        DILAY_IMPOSSIBLE
    }
  }

  MovementFixedConstraint constraintFromInt (int i)
  {
    switch (i)
    {
      case 0:
        return MovementFixedConstraint::XAxis;
      case 1:
        return MovementFixedConstraint::YAxis;
      case 2:
        return MovementFixedConstraint::ZAxis;
      case 3:
        return MovementFixedConstraint::XYPlane;
      case 4:
        return MovementFixedConstraint::XZPlane;
      case 5:
        return MovementFixedConstraint::YZPlane;
      case 6:
        return MovementFixedConstraint::CameraPlane;
      case 7:
        return MovementFixedConstraint::PrimaryPlane;
      default:
        DILAY_IMPOSSIBLE
    }
  }
}

struct ToolUtilMovement::Impl
{
  const Camera& camera;
  glm::vec3     previousPosition;
  glm::vec3     position;

  Variant<MovementFixedConstraint, glm::vec3> constraint;

  Impl (const Camera& cam)
    : camera (cam)
    , previousPosition (glm::vec3 (0.0f))
    , position (glm::vec3 (0.0f))
  {
  }

  // Constrained to fixed plane or axis
  Impl (const Camera& cam, MovementFixedConstraint c)
    : Impl (cam)
  {
    this->constraint.set (c);
  }

  MovementFixedConstraint fixedConstraint () const
  {
    return this->constraint.get<MovementFixedConstraint> ();
  }

  void fixedConstraint (MovementFixedConstraint c)
  {
    this->constraint.set (c);
  }

  void addFixedProperties (ViewTwoColumnGrid& properties, const std::function<void()>& onClick)
  {
    QButtonGroup& constraintEdit = *new QButtonGroup;
    properties.add (constraintEdit,
                    {QObject::tr ("X-axis"), QObject::tr ("Y-axis"), QObject::tr ("Z-axis"),
                     QObject::tr ("XY-plane"), QObject::tr ("XZ-plane"), QObject::tr ("YZ-plane"),
                     QObject::tr ("Camera-plane"), QObject::tr ("Primary plane")});
    ViewUtil::connect (constraintEdit, [this, onClick](int id) {
      this->constraint.set (constraintFromInt (id));
      onClick ();
    });
    constraintEdit.button (constraintToInt (this->constraint.get<MovementFixedConstraint> ()))
      ->click ();
  }

  // Constrained to free plane
  Impl (const Camera& cam, const glm::vec3& c)
    : Impl (cam)
  {
    this->constraint.set (c);
  }

  const glm::vec3& freePlaneConstraint () const
  {
    return this->constraint.get<glm::vec3> ();
  }

  void freePlaneConstraint (const glm::vec3& c)
  {
    this->constraint.set (c);
  }

  glm::vec3 delta () const
  {
    return this->position - this->previousPosition;
  }

  bool intersects (const glm::vec3& normal, const glm::ivec2& p, glm::vec3& i) const
  {
    const PrimRay   ray = this->camera.ray (p);
    const PrimPlane plane (this->position, normal);
    float           t;

    if (IntersectionUtil::intersects (ray, plane, &t))
    {
      i = ray.pointAt (t);
      return true;
    }
    return false;
  }

  bool moveOnPlane (const glm::vec3& normal, const glm::ivec2& p)
  {
    glm::vec3 i;
    if (this->intersects (normal, p, i))
    {
      this->previousPosition = this->position;
      this->position = i;
      return true;
    }
    else
    {
      return false;
    }
  }

  bool moveOnPlane (Dimension normalDim, const glm::ivec2& p)
  {
    glm::vec3 normal (0.0f);
    normal[DimensionUtil::index (normalDim)] = 1.0f;
    return this->moveOnPlane (normal, p);
  }

  bool moveAlong (Dimension axisDim, const glm::ivec2& p)
  {
    const unsigned int i = DimensionUtil::index (axisDim);
    const glm::vec3    e = this->camera.toEyePoint ();

    if (glm::dot (DimensionUtil::vector (axisDim), glm::normalize (e)) < 0.9)
    {
      glm::vec3 normal = e;
      normal[i] = 0.0f;
      glm::vec3 intersection;

      if (this->intersects (normal, p, intersection))
      {
        this->previousPosition = this->position;
        this->position[i] = intersection[i];
        return true;
      }
    }
    return false;
  }

  bool move (const glm::ivec2& p, bool modify)
  {
    if (this->constraint.is<MovementFixedConstraint> ())
    {
      switch (this->constraint.get<MovementFixedConstraint> ())
      {
        case MovementFixedConstraint::XAxis:
          return modify ? this->moveOnPlane (Dimension::X, p) : this->moveAlong (Dimension::X, p);

        case MovementFixedConstraint::YAxis:
          return modify ? this->moveOnPlane (Dimension::Y, p) : this->moveAlong (Dimension::Y, p);

        case MovementFixedConstraint::ZAxis:
          return modify ? this->moveOnPlane (Dimension::Z, p) : this->moveAlong (Dimension::Z, p);

        case MovementFixedConstraint::XYPlane:
          return modify ? this->moveAlong (Dimension::Z, p) : this->moveOnPlane (Dimension::Z, p);

        case MovementFixedConstraint::XZPlane:
          return modify ? this->moveAlong (Dimension::Y, p) : this->moveOnPlane (Dimension::Y, p);

        case MovementFixedConstraint::YZPlane:
          return modify ? this->moveAlong (Dimension::X, p) : this->moveOnPlane (Dimension::X, p);

        case MovementFixedConstraint::CameraPlane:
          return this->moveOnPlane (this->camera.toEyePoint (), p);

        case MovementFixedConstraint::PrimaryPlane:
          return modify ? this->moveAlong (this->camera.primaryDimension (), p)
                        : this->moveOnPlane (this->camera.primaryDimension (), p);

        default:
          DILAY_IMPOSSIBLE
      }
    }
    else
    {
      return this->moveOnPlane (this->constraint.get<glm::vec3> (), p);
    }
  }

  bool move (const ViewPointingEvent& e, bool considerShift)
  {
    return this->move (e.ivec2 (), considerShift && e.modifiers () == Qt::ShiftModifier);
  }

  void resetPosition (const glm::vec3& p)
  {
    this->previousPosition = p;
    this->position = p;
  }
};

template <> void KVStore::set (const std::string& path, const MovementFixedConstraint& c)
{
  this->set (path, constraintToInt (c));
}

template <>
MovementFixedConstraint KVStore::getFrom (const std::string&             path,
                                          const MovementFixedConstraint& c) const
{
  return constraintFromInt (this->get (path, constraintToInt (c)));
}

DELEGATE2_BIG3 (ToolUtilMovement, const Camera&, MovementFixedConstraint)
DELEGATE_CONST (MovementFixedConstraint, ToolUtilMovement, fixedConstraint)
DELEGATE1 (void, ToolUtilMovement, fixedConstraint, MovementFixedConstraint)
DELEGATE2 (void, ToolUtilMovement, addFixedProperties, ViewTwoColumnGrid&,
           const std::function<void()>&)

DELEGATE2_CONSTRUCTOR (ToolUtilMovement, const Camera&, const glm::vec3&)
DELEGATE_CONST (const glm::vec3&, ToolUtilMovement, freePlaneConstraint)
DELEGATE1 (void, ToolUtilMovement, freePlaneConstraint, const glm::vec3&)

DELEGATE_CONST (glm::vec3, ToolUtilMovement, delta)
GETTER_CONST (const glm::vec3&, ToolUtilMovement, position)
SETTER (const glm::vec3&, ToolUtilMovement, position)
DELEGATE2 (bool, ToolUtilMovement, move, const ViewPointingEvent&, bool)
DELEGATE1 (void, ToolUtilMovement, resetPosition, const glm::vec3&)
