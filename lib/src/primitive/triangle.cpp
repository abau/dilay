#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/norm.hpp>
#include <sstream>
#include "primitive/triangle.hpp"
#include "ray.hpp"
#include "util.hpp"
#include "winged/vertex.hpp"

PrimTriangle :: PrimTriangle (const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) 
  : _vertex1 (v1)
  , _vertex2 (v2)
  , _vertex3 (v3) 
{}

PrimTriangle :: PrimTriangle ( const WingedMesh& mesh, const WingedVertex& v1
                             , const WingedVertex& v2, const WingedVertex& v3)
  : PrimTriangle ( v1.position (mesh)
                 , v2.position (mesh)
                 , v3.position (mesh) ) 
{}

glm::vec3 PrimTriangle :: cross () const { 
  assert (this->isDegenerated () == false);
  return glm::cross ( this->_vertex2 - this->_vertex1
                    , this->_vertex3 - this->_vertex2 );
}

glm::vec3 PrimTriangle :: normal () const { 
  return glm::normalize (this->cross ());
}

glm::vec3 PrimTriangle :: center () const {
  return (this->_vertex1 + this->_vertex2 + this->_vertex3) / glm::vec3 (3.0f);
}

glm::vec3 PrimTriangle :: minimum () const {
  return glm::min (glm::min (this->_vertex1, this->_vertex2), this->_vertex3);
}

glm::vec3 PrimTriangle :: maximum () const {
  return glm::max (glm::max (this->_vertex1, this->_vertex2), this->_vertex3);
}

float PrimTriangle :: extent () const {
  return glm::length (this->maximum () - this->minimum ());
}

float PrimTriangle :: oneDimExtent () const {
  const glm::vec3 extent = this->maximum () - this->minimum ();
  return glm::max (glm::max (extent.x, extent.y), extent.z);
}

bool PrimTriangle :: isDegenerated () const {
  const float d1 = glm::distance (this->_vertex1, this->_vertex2);
  const float d2 = glm::distance (this->_vertex2, this->_vertex3);
  const float d3 = glm::distance (this->_vertex1, this->_vertex3);

  return glm::epsilonEqual (d1, d2 + d3, Util::epsilon ())
      || glm::epsilonEqual (d2, d1 + d3, Util::epsilon ())
      || glm::epsilonEqual (d3, d1 + d2, Util::epsilon ());
}

float PrimTriangle :: incircleRadiusSqr () const {
  assert (this->isDegenerated () == false);

  const float a = glm::distance (this->_vertex1,this->_vertex2);
  const float b = glm::distance (this->_vertex2,this->_vertex3);
  const float c = glm::distance (this->_vertex1,this->_vertex3);
  const float s = 0.5f * (a + b + c);
  return (s-a) * (s-b) * (s-c) / s;
}

float PrimTriangle :: longestEdgeSqr () const {
  return glm::max ( glm::max ( glm::distance2 (this->_vertex1,this->_vertex2)
                             , glm::distance2 (this->_vertex2,this->_vertex3) )
                             , glm::distance2 (this->_vertex1,this->_vertex3) );
}

std::ostream& operator<<(std::ostream& os, const PrimTriangle& triangle) {
  os << "PrimTriangle { " << triangle.vertex1 ()
                  << ", " << triangle.vertex2 ()
                  << ", " << triangle.vertex3 ()
                  << " }";
  return os;
}
