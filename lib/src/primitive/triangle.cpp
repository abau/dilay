#include <glm/glm.hpp>
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

  glm::vec3 edge1 () const { return this->vertex2 - this->vertex1; }
  glm::vec3 edge2 () const { return this->vertex3 - this->vertex2; }

  glm::vec3 normal () const { 
    return glm::normalize (glm::cross (this->edge1 (), this->edge2 ()));
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
};

DELEGATE3_BIG6         (PrimTriangle, const glm::vec3&, const glm::vec3&, const glm::vec3&)
DELEGATE4_CONSTRUCTOR  (PrimTriangle, const WingedMesh&, const WingedVertex&, const WingedVertex&, const WingedVertex&)

GETTER_CONST    (const glm::vec3&  , PrimTriangle, vertex1)
GETTER_CONST    (const glm::vec3&  , PrimTriangle, vertex2)
GETTER_CONST    (const glm::vec3&  , PrimTriangle, vertex3)

DELEGATE_CONST  (glm::vec3         , PrimTriangle, edge1)
DELEGATE_CONST  (glm::vec3         , PrimTriangle, edge2)
DELEGATE_CONST  (glm::vec3         , PrimTriangle, normal)
DELEGATE_CONST  (glm::vec3         , PrimTriangle, center)
DELEGATE_CONST  (glm::vec3         , PrimTriangle, minimum)
DELEGATE_CONST  (glm::vec3         , PrimTriangle, maximum)
DELEGATE_CONST  (float             , PrimTriangle, extent)
DELEGATE_CONST  (float             , PrimTriangle, oneDimExtent)
