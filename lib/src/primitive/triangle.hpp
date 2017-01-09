/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PRIMITIVE_TRIANGLE
#define DILAY_PRIMITIVE_TRIANGLE

#include <glm/glm.hpp>
#include <iosfwd>

class PrimTriangle {
  public:
    PrimTriangle (const glm::vec3&, const glm::vec3&, const glm::vec3&);

    const glm::vec3& vertex1           () const { return this->_vertex1; }
    const glm::vec3& vertex2           () const { return this->_vertex2; }
    const glm::vec3& vertex3           () const { return this->_vertex3; }
          glm::vec3  cross             () const;
          glm::vec3  normal            () const;
          glm::vec3  center            () const;
          glm::vec3  minimum           () const;
          glm::vec3  maximum           () const;
          float      maxExtent         () const;
          float      maxDimExtent      () const;
          float      incircleRadiusSqr () const;
          float      longestEdgeSqr    () const;

  private:
    const glm::vec3& _vertex1;
    const glm::vec3& _vertex2;
    const glm::vec3& _vertex3;
};

std::ostream& operator<<(std::ostream&, const PrimTriangle&);

#endif
