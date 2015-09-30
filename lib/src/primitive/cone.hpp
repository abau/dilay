/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PRIMITIVE_CONE
#define DILAY_PRIMITIVE_CONE

#include <glm/glm.hpp>
#include <iosfwd>

class PrimCone {
  public:
    PrimCone (const glm::vec3&, float, const glm::vec3&, float);

    const glm::vec3& center1     () const { return this->_center1; }
    float            radius1     () const { return this->_radius1; }
    const glm::vec3& center2     () const { return this->_center2; }
    float            radius2     () const { return this->_radius2; }
    const glm::vec3& direction   () const { return this->_direction; }
    bool             isCylinder  () const { return this->_isCylinder; }
    const glm::vec3& apex        () const { return this->_apex; }
    float            alpha       () const { return this->_alpha; }
    float            sinSqrAlpha () const { return this->_sinSqrAlpha; }
    float            cosSqrAlpha () const { return this->_cosSqrAlpha; }

  private:
    const glm::vec3 _center1;
    const float     _radius1;
    const glm::vec3 _center2;
    const float     _radius2;
    const glm::vec3 _direction;
    const bool      _isCylinder;
    const glm::vec3 _apex;
    const float     _alpha;
    const float     _sinSqrAlpha;
    const float     _cosSqrAlpha;
};

std::ostream& operator<<(std::ostream&, const PrimCone&);

#endif
