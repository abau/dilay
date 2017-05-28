/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "tool/util/step.hpp"

struct ToolUtilStep::Impl
{
  float stepWidth;

  Impl ()
    : stepWidth (0.0f)
  {
  }

  glm::vec3 step (const glm::vec3& from, const glm::vec3& to,
                  const std::function<bool(const glm::vec3&)>& f)
  {
    assert (this->stepWidth >= 0.0f);

    glm::vec3 position = from;

    do
    {
      const glm::vec3 direction = to - position;
      const float     distance = glm::length (direction);

      if (distance < this->stepWidth)
      {
        break;
      }
      position += direction * (this->stepWidth / distance);
    } while (f (position));

    return position;
  }
};

DELEGATE_BIG3 (ToolUtilStep)
SETTER (float, ToolUtilStep, stepWidth)
DELEGATE3 (glm::vec3, ToolUtilStep, step, const glm::vec3&, const glm::vec3&,
           const std::function<bool(const glm::vec3&)>&)
