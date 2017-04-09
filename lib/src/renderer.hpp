/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_RENDERER
#define DILAY_RENDERER

#include <glm/fwd.hpp>
#include "configurable.hpp"
#include "macro.hpp"

class Color;
class Config;
class RenderMode;

class Renderer : public Configurable
{
public:
  DECLARE_BIG3 (Renderer, const Config&)

  void setupRendering ();
  void shutdownRendering ();
  void setProgram (const RenderMode&);
  void setModel (const float*, const float*);
  void setView (const float*);
  void setProjection (const float*);
  void setColor (const Color&, bool = false);
  void setWireframeColor (const Color&, bool = false);
  void setEyePoint (const glm::vec3&);
  void setLightDirection (unsigned int, const glm::vec3&);
  void setLightColor (unsigned int, const Color&);
  void setLightIrradiance (unsigned int, float);

private:
  IMPLEMENTATION

  void runFromConfig (const Config&);
};

#endif
