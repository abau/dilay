/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "tool/util/step.hpp"
#include "util.hpp"

namespace
{
  const glm::vec3 invalidPosition (Util::maxFloat ());
}

struct ToolUtilStep::Impl
{
  glm::vec3 position;
  float     stepWidth;

  Impl ()
    : position (invalidPosition)
    , stepWidth (0.0f)
  {
  }

  void step (const glm::vec3& to, const std::function<bool(const glm::vec3&)>& f)
  {
    assert (this->position != invalidPosition);
    assert (this->stepWidth >= 0.0f);

    do
    {
      const glm::vec3 direction = to - this->position;
      const float     distance = glm::length (direction);

      if (distance < this->stepWidth)
      {
        break;
      }
      this->position += direction * (this->stepWidth / distance);
    } while (f (this->position));
  }
};

DELEGATE_BIG3 (ToolUtilStep)
GETTER_CONST (const glm::vec3&, ToolUtilStep, position)
SETTER (const glm::vec3&, ToolUtilStep, position)
SETTER (float, ToolUtilStep, stepWidth)
DELEGATE2 (void, ToolUtilStep, step, const glm::vec3&, const std::function<bool(const glm::vec3&)>&)
