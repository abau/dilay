#ifndef DILAY_PRIMITIVE_TRIANGLE
#define DILAY_PRIMITIVE_TRIANGLE

#include <glm/fwd.hpp>
#include "macro.hpp"

class Ray;
class WingedMesh;
class WingedVertex;

class PrimTriangle {
  public:
    DECLARE_BIG4COPY (PrimTriangle, const glm::vec3&, const glm::vec3&, const glm::vec3&);
    PrimTriangle ( const WingedMesh&, const WingedVertex&
                 , const WingedVertex&, const WingedVertex&);

    const glm::vec3&  vertex1      () const;
    const glm::vec3&  vertex2      () const;
    const glm::vec3&  vertex3      () const;

    glm::vec3         normal       () const;
    glm::vec3         center       () const;
    glm::vec3         minimum      () const;
    glm::vec3         maximum      () const;
    float             extent       () const;
    float             oneDimExtent () const;

  private:
    IMPLEMENTATION
};

#endif
