/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"
#include "tool/util/rotation.hpp"
#include "util.hpp"
#include "view/pointing-event.hpp"

struct ToolUtilRotation::Impl
{
  const Camera& camera;
  glm::vec3     origin;
  glm::vec3     axis;
  glm::vec2     position;
  glm::vec2     direction;
  float         angle;

  Impl (const Camera& cam)
    : camera (cam)
  {
  }

  glm::mat4x4 matrix () const
  {
    const glm::mat4x4 translate = glm::translate (glm::mat4x4 (1.0f), this->origin);
    const glm::mat4x4 rotate = glm::rotate (translate, this->angle, this->axis);
    return glm::translate (rotate, -this->origin);
  }

  bool rotate (const ViewPointingEvent& e)
  {
    const glm::vec2 fPoint = glm::vec2 (e.position ());
    const float     threshold = 10.0f;

    if (glm::distance (this->position, fPoint) < threshold)
    {
      this->direction = glm::vec2 (0.0f);
      return false;
    }
    else if (this->direction == glm::vec2 (0.0f))
    {
      this->direction = glm::normalize (fPoint - this->position);
      return false;
    }
    else
    {
      const glm::vec2 newDirection = glm::normalize (fPoint - this->position);
      const float     dot = glm::dot (this->direction, newDirection);

      if (Util::almostEqual (dot, 1.0f))
      {
        this->angle = 0.0f;
      }
      else if (Util::almostEqual (dot, -1.0f))
      {
        this->angle = glm::pi<float> ();
      }
      else
      {
        this->angle = glm::acos (dot);

        if (Util::cross (this->direction, newDirection) > 0.0f)
        {
          this->angle = glm::two_pi<float> () - this->angle;
        }
      }
      this->direction = newDirection;
      return true;
    }
  }

  void reset (const glm::vec3& o)
  {
    this->origin = o;
    this->axis = this->camera.toEyePoint ();
    this->position = this->camera.fromWorld (o, glm::mat4x4 (1.0f), false);
    this->direction = glm::vec2 (0.0f);
  }

  void reset (const glm::vec3& o, const glm::vec3& a)
  {
    this->origin = o;
    this->axis = a;
    this->position = this->camera.fromWorld (o, glm::mat4x4 (1.0f), false);
    this->direction = glm::vec2 (0.0f);
  }
};

DELEGATE1_BIG3 (ToolUtilRotation, const Camera&)
DELEGATE_CONST (glm::mat4x4, ToolUtilRotation, matrix)
DELEGATE1 (bool, ToolUtilRotation, rotate, const ViewPointingEvent&)
DELEGATE1 (void, ToolUtilRotation, reset, const glm::vec3&)
DELEGATE2 (void, ToolUtilRotation, reset, const glm::vec3&, const glm::vec3&)
