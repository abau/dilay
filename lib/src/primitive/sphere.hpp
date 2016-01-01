/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PRIMITIVE_SPHERE
#define DILAY_PRIMITIVE_SPHERE

#include <glm/glm.hpp>
#include <iosfwd>

class PrimSphere {
  public:
    PrimSphere (const glm::vec3&, float);

    const glm::vec3& center () const { return this->_center; }
    float            radius () const { return this->_radius; }

    void center (const glm::vec3& c) { this->_center = c; }
    void radius (float r)            { this->_radius = r; }

  private:
    glm::vec3 _center;
    float     _radius;
};

std::ostream& operator<<(std::ostream&, const PrimSphere&);

#endif
