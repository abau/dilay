/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PRIMITIVE_AABOX
#define DILAY_PRIMITIVE_AABOX

#include <glm/glm.hpp>

class PrimAABox {
  public:
    PrimAABox (const glm::vec3&, const glm::vec3&);
    PrimAABox (const glm::vec3&, float, float, float);
    PrimAABox (const glm::vec3&, float);

    const glm::vec3& maximum  () const { return this->_maximum; }
    const glm::vec3& minimum  () const { return this->_minimum; }
    const glm::vec3& center   () const { return this->_center ; }

          float      xWidth   () const;
          float      yWidth   () const;
          float      zWidth   () const;

  private:
    const glm::vec3 _minimum;
    const glm::vec3 _maximum;
    const glm::vec3 _center;
};

#endif
