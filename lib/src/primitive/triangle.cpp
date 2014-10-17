#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <sstream>
#include "primitive/triangle.hpp"
#include "ray.hpp"
#include "util.hpp"
#include "winged/vertex.hpp"

struct PrimTriangle::Impl {
  const glm::vec3 vertex1;
  const glm::vec3 vertex2;
  const glm::vec3 vertex3;

  Impl (const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) 
    : vertex1 (v1), vertex2 (v2), vertex3 (v3) {}

  Impl ( const WingedMesh& mesh, const WingedVertex& v1
               , const WingedVertex& v2, const WingedVertex& v3)
    : Impl (v1.vector (mesh), v2.vector (mesh), v3.vector (mesh)) {}

  glm::vec3 normal () const { 
    assert (this->isDegenerated () == false);
    return glm::normalize (glm::cross ( this->vertex2 - this->vertex1
                                      , this->vertex3 - this->vertex2 ));
  }

  glm::vec3 center () const {
    return (this->vertex1 + this->vertex2 + this->vertex3) / glm::vec3 (3.0f);
  }

  glm::vec3 minimum () const {
    return glm::min (glm::min (this->vertex1, this->vertex2), this->vertex3);
  }

  glm::vec3 maximum () const {
    return glm::max (glm::max (this->vertex1, this->vertex2), this->vertex3);
  }

  float extent () const {
    return glm::length (this->maximum () - this->minimum ());
  }

  float oneDimExtent () const {
    glm::vec3 delta = this->maximum () - this->minimum ();
    return glm::max ( glm::max (delta.x, delta.y), delta.z );
  }

  bool isDegenerated () const {
    const float d1 = glm::distance (this->vertex1, this->vertex2);
    const float d2 = glm::distance (this->vertex2, this->vertex3);
    const float d3 = glm::distance (this->vertex1, this->vertex3);

    return glm::epsilonEqual (d1, d2 + d3, Util::epsilon ())
        || glm::epsilonEqual (d2, d1 + d3, Util::epsilon ())
        || glm::epsilonEqual (d3, d1 + d2, Util::epsilon ());
  }
};

DELEGATE3_BIG4COPY     (PrimTriangle, const glm::vec3&, const glm::vec3&, const glm::vec3&)
DELEGATE4_CONSTRUCTOR  (PrimTriangle, const WingedMesh&, const WingedVertex&, const WingedVertex&, const WingedVertex&)

GETTER_CONST    (const glm::vec3&  , PrimTriangle, vertex1)
GETTER_CONST    (const glm::vec3&  , PrimTriangle, vertex2)
GETTER_CONST    (const glm::vec3&  , PrimTriangle, vertex3)

DELEGATE_CONST  (glm::vec3         , PrimTriangle, normal)
DELEGATE_CONST  (glm::vec3         , PrimTriangle, center)
DELEGATE_CONST  (glm::vec3         , PrimTriangle, minimum)
DELEGATE_CONST  (glm::vec3         , PrimTriangle, maximum)
DELEGATE_CONST  (float             , PrimTriangle, extent)
DELEGATE_CONST  (float             , PrimTriangle, oneDimExtent)
DELEGATE_CONST  (bool              , PrimTriangle, isDegenerated)

std::ostream& operator<<(std::ostream& os, const PrimTriangle& triangle) {
  os << "PrimTriangle { " << triangle.vertex1 ()
                  << ", " << triangle.vertex2 ()
                  << ", " << triangle.vertex3 ()
                  << " }";
  return os;
}
