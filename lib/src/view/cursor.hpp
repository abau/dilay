/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_VIEW_CURSOR
#define DILAY_VIEW_CURSOR

#include <glm/fwd.hpp>
#include "macro.hpp"

class Camera;
class Color;
class Mesh;

class ViewCursor
{
public:
  DECLARE_BIG6 (ViewCursor)

  float        radius () const;
  glm::vec3    position () const;
  const Color& color () const;
  bool         isEnabled () const;

  void radius (float);
  void position (const glm::vec3&);
  void color (const Color&);
  void enable ();
  void disable ();
  void render (Camera&) const;

private:
  IMPLEMENTATION
};

#endif
