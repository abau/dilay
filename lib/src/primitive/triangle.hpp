#ifndef DILAY_PRIMITIVE_TRIANGLE
#define DILAY_PRIMITIVE_TRIANGLE

#include <glm/glm.hpp>
#include <iosfwd>

class Ray;
class WingedMesh;
class WingedVertex;

class PrimTriangle {
  public:
    PrimTriangle (const glm::vec3&, const glm::vec3&, const glm::vec3&);
    PrimTriangle ( const WingedMesh&, const WingedVertex&
                 , const WingedVertex&, const WingedVertex&);

    const glm::vec3& vertex1           () const { return this->_vertex1; }
    const glm::vec3& vertex2           () const { return this->_vertex2; }
    const glm::vec3& vertex3           () const { return this->_vertex3; }
          glm::vec3  normal            () const;
          glm::vec3  center            () const;
          glm::vec3  minimum           () const;
          glm::vec3  maximum           () const;
          float      extent            () const;
          float      oneDimExtent      () const;
          bool       isDegenerated     () const;
          float      incircleRadiusSqr () const;
          float      longestEdgeSqr    () const;

  private:
    const glm::vec3 _vertex1;
    const glm::vec3 _vertex2;
    const glm::vec3 _vertex3;
};

std::ostream& operator<<(std::ostream&, const PrimTriangle&);

#endif
