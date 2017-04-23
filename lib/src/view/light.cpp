/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "color.hpp"
#include "view/light.hpp"

struct ViewLight::Impl
{
  glm::vec3 position;
  Color     color;
  float     irradiance;

  Impl (const glm::vec3& p, const Color& c, float i)
    : position (p)
    , color (c)
    , irradiance (i)
  {
  }
};

DELEGATE3_BIG6 (ViewLight, const glm::vec3&, const Color&, float)

GETTER_CONST (const glm::vec3&, ViewLight, position)
GETTER_CONST (const Color&, ViewLight, color)
GETTER_CONST (float, ViewLight, irradiance)

SETTER (const glm::vec3&, ViewLight, position)
SETTER (const Color&, ViewLight, color)
SETTER (float, ViewLight, irradiance)
