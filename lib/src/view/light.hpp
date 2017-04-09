/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_LIGHT
#define DILAY_VIEW_LIGHT

#include <glm/fwd.hpp>
#include "macro.hpp"

class Color;

class ViewLight
{
public:
  DECLARE_BIG6 (ViewLight, const glm::vec3&, const Color&, float);

  const glm::vec3& position () const;
  const Color&     color () const;
  float            irradiance () const;

  void position (const glm::vec3&);
  void color (const Color&);
  void irradiance (float);

private:
  IMPLEMENTATION
};

#endif
