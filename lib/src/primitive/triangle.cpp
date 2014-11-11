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
    : Impl (v1.position (mesh), v2.position (mesh), v3.position (mesh)) {}

  glm::vec3 normal () const { 
    return PrimTriangle::normal (this->vertex1, this->vertex2, this->vertex3);
  }

  glm::vec3 center () const { 
    return PrimTriangle::center (this->vertex1, this->vertex2, this->vertex3);
  }

  glm::vec3 minimum () const { 
    return PrimTriangle::minimum (this->vertex1, this->vertex2, this->vertex3);
  }

  glm::vec3 maximum () const { 
    return PrimTriangle::maximum (this->vertex1, this->vertex2, this->vertex3);
  }

  float extent () const { 
    return PrimTriangle::extent (this->vertex1, this->vertex2, this->vertex3);
  }

  float oneDimExtent () const { 
    return PrimTriangle::oneDimExtent (this->vertex1, this->vertex2, this->vertex3);
  }

  bool isDegenerated () const { 
    return PrimTriangle::isDegenerated (this->vertex1, this->vertex2, this->vertex3);
  }

  float incircleRadiusSqr () const { 
    return PrimTriangle::incircleRadiusSqr (this->vertex1, this->vertex2, this->vertex3);
  }

  static glm::vec3 normal (const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) { 
    assert (PrimTriangle::isDegenerated (v1,v2,v3) == false);
    return glm::normalize (glm::cross (v2 - v1, v3 - v2));
  }

  static glm::vec3 center (const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
    return (v1 + v2 + v3) / glm::vec3 (3.0f);
  }

  static glm::vec3 minimum (const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
    return glm::min (glm::min (v1, v2), v3);
  }

  static glm::vec3 maximum (const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
    return glm::max (glm::max (v1, v2), v3);
  }

  static float extent (const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
    return glm::length (PrimTriangle::maximum (v1,v2,v3) - PrimTriangle::minimum (v1,v2,v3));
  }

  static float oneDimExtent (const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
    glm::vec3 delta = PrimTriangle::maximum (v1,v2,v3) - PrimTriangle::minimum (v1,v2,v3);
    return glm::max ( glm::max (delta.x, delta.y), delta.z );
  }

  static bool isDegenerated (const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
    const float d1 = glm::distance (v1, v2);
    const float d2 = glm::distance (v2, v3);
    const float d3 = glm::distance (v1, v3);

    return glm::epsilonEqual (d1, d2 + d3, Util::epsilon ())
        || glm::epsilonEqual (d2, d1 + d3, Util::epsilon ())
        || glm::epsilonEqual (d3, d1 + d2, Util::epsilon ());
  }

  static float incircleRadiusSqr (const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
    assert (PrimTriangle::isDegenerated (v1,v2,v3) == false);

    const float a = glm::distance (v1,v2);
    const float b = glm::distance (v2,v3);
    const float c = glm::distance (v1,v3);
    const float s = 0.5f * (a + b + c);
    return (s-a) * (s-b) * (s-c) / s;
  }
};

DELEGATE3_BIG4COPY     (PrimTriangle, const glm::vec3&, const glm::vec3&, const glm::vec3&)
DELEGATE4_CONSTRUCTOR  (PrimTriangle, const WingedMesh&, const WingedVertex&, const WingedVertex&, const WingedVertex&)

GETTER_CONST     (const glm::vec3&  , PrimTriangle, vertex1)
GETTER_CONST     (const glm::vec3&  , PrimTriangle, vertex2)
GETTER_CONST     (const glm::vec3&  , PrimTriangle, vertex3)

DELEGATE_CONST   (glm::vec3         , PrimTriangle, normal)
DELEGATE_CONST   (glm::vec3         , PrimTriangle, center)
DELEGATE_CONST   (glm::vec3         , PrimTriangle, minimum)
DELEGATE_CONST   (glm::vec3         , PrimTriangle, maximum)
DELEGATE_CONST   (float             , PrimTriangle, extent)
DELEGATE_CONST   (float             , PrimTriangle, oneDimExtent)
DELEGATE_CONST   (bool              , PrimTriangle, isDegenerated)
DELEGATE_CONST   (float             , PrimTriangle, incircleRadiusSqr)

DELEGATE3_STATIC (glm::vec3         , PrimTriangle, normal, const glm::vec3&, const glm::vec3&, const glm::vec3&)
DELEGATE3_STATIC (glm::vec3         , PrimTriangle, center, const glm::vec3&, const glm::vec3&, const glm::vec3&)
DELEGATE3_STATIC (glm::vec3         , PrimTriangle, minimum, const glm::vec3&, const glm::vec3&, const glm::vec3&)
DELEGATE3_STATIC (glm::vec3         , PrimTriangle, maximum, const glm::vec3&, const glm::vec3&, const glm::vec3&)
DELEGATE3_STATIC (float             , PrimTriangle, extent, const glm::vec3&, const glm::vec3&, const glm::vec3&)
DELEGATE3_STATIC (float             , PrimTriangle, oneDimExtent, const glm::vec3&, const glm::vec3&, const glm::vec3&)
DELEGATE3_STATIC (bool              , PrimTriangle, isDegenerated, const glm::vec3&, const glm::vec3&, const glm::vec3&)
DELEGATE3_STATIC (float             , PrimTriangle, incircleRadiusSqr, const glm::vec3&, const glm::vec3&, const glm::vec3&)

std::ostream& operator<<(std::ostream& os, const PrimTriangle& triangle) {
  os << "PrimTriangle { " << triangle.vertex1 ()
                  << ", " << triangle.vertex2 ()
                  << ", " << triangle.vertex3 ()
                  << " }";
  return os;
}
