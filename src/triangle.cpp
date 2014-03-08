#include <glm/glm.hpp>
#include "triangle.hpp"
#include "util.hpp"
#include "ray.hpp"
#include "winged/vertex.hpp"

struct Triangle::Impl {
  glm::vec3 vertex1;
  glm::vec3 vertex2;
  glm::vec3 vertex3;

  Impl () {}

  Impl (const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) 
    : vertex1 (v1), vertex2 (v2), vertex3 (v3) {}

  Impl ( const WingedMesh& mesh, const WingedVertex& v1
               , const WingedVertex& v2, const WingedVertex& v3)
    : Impl (v1.vertex (mesh), v2.vertex (mesh), v3.vertex (mesh)) {}

  glm::vec3 edge1 () const { return this->vertex2 - this->vertex1; }
  glm::vec3 edge2 () const { return this->vertex3 - this->vertex1; }

  glm::vec3 normal () const { 
    return glm::cross (this->edge1 (), this->edge2 ());
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

  float maxExtent () const {
    glm::vec3 delta = this->maximum () - this->minimum ();
    return glm::max ( glm::max (delta.x, delta.y), delta.z );
  }
};

DELEGATE_BIG6          (Triangle)
DELEGATE3_CONSTRUCTOR  (Triangle, const glm::vec3&, const glm::vec3&, const glm::vec3&)
DELEGATE4_CONSTRUCTOR  (Triangle, const WingedMesh&, const WingedVertex&, const WingedVertex&, const WingedVertex&)

GETTER_CONST    (const glm::vec3&  , Triangle, vertex1)
GETTER_CONST    (const glm::vec3&  , Triangle, vertex2)
GETTER_CONST    (const glm::vec3&  , Triangle, vertex3)

SETTER          (const glm::vec3&  , Triangle, vertex1)
SETTER          (const glm::vec3&  , Triangle, vertex2)
SETTER          (const glm::vec3&  , Triangle, vertex3)

DELEGATE_CONST  (glm::vec3         , Triangle, edge1)
DELEGATE_CONST  (glm::vec3         , Triangle, edge2)
DELEGATE_CONST  (glm::vec3         , Triangle, normal)
DELEGATE_CONST  (glm::vec3         , Triangle, center)
DELEGATE_CONST  (glm::vec3         , Triangle, minimum)
DELEGATE_CONST  (glm::vec3         , Triangle, maximum)
DELEGATE_CONST  (float             , Triangle, maxExtent)
