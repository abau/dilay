/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "sketch/sphere.hpp"

SketchSphere :: SketchSphere (unsigned int i, const glm::vec3& p, float r)
  : _index  (i)
  , _sphere (p, r)
{}

unsigned int      SketchSphere :: index  () const { return this->_index;            }
const PrimSphere& SketchSphere :: sphere () const { return this->_sphere;           }
const glm::vec3&  SketchSphere :: center () const { return this->_sphere.center (); }
float             SketchSphere :: radius () const { return this->_sphere.radius (); }

void SketchSphere :: center (const glm::vec3& p) { this->_sphere.center (p); }
void SketchSphere :: radius (float r)            { this->_sphere.radius (r); }
