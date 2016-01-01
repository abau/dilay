/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PRIMITIVE_CONE_SPHERE
#define DILAY_PRIMITIVE_CONE_SPHERE

#include <glm/glm.hpp>
#include <iosfwd>
#include "primitive/sphere.hpp"

class PrimCone;

class PrimConeSphere {
  public:
    PrimConeSphere (const PrimSphere&, const PrimSphere&);

    const PrimSphere& sphere1        () const { return this->_sphere1; }
    const PrimSphere& sphere2        () const { return this->_sphere2; }
          float       length         () const { return this->_length; }
    const glm::vec3&  direction      () const { return this->_direction; }
          float       alpha          () const { return this->_alpha; }
          float       sinAlpha       () const { return this->_sinAlpha; }
          float       cosAlpha       () const { return this->_cosAlpha; }
          float       delta          () const;
          bool        sameRadii      () const;
          bool        hasCone        () const;
          float       coneSideLength () const;
          PrimCone    toCone         () const;

  private:
    const PrimSphere _sphere1;
    const PrimSphere _sphere2;
    const float      _length;
    const glm::vec3  _direction;
    const float      _alpha;
    const float      _sinAlpha;
    const float      _cosAlpha;
};

std::ostream& operator<<(std::ostream&, const PrimConeSphere&);

#endif
