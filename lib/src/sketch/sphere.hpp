/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SKETCH_SPHERE
#define DILAY_SKETCH_SPHERE

#include "primitive/sphere.hpp"

class SketchSphere {
  public:
    SketchSphere (unsigned int, const glm::vec3&, float);

    unsigned int      index  () const;
    const PrimSphere& sphere () const;
    const glm::vec3&  center () const;
    float             radius () const;

    void center (const glm::vec3& p);
    void radius (float r);

  private:
    unsigned int _index;
    PrimSphere   _sphere;
};

#endif
