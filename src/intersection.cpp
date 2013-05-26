#include <cmath>
#include <glm/glm.hpp>
#include "intersection.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "macro.hpp"
#include "sphere.hpp"
#include "adjacent-iterator.hpp"

struct FaceIntersectionImpl {
  bool       isIntersection;
  float      distance;
  glm::vec3  position;
  LinkedFace face;

  FaceIntersectionImpl () : isIntersection (false) {}

  void update (float d, const glm::vec3& p, LinkedFace f) {
    if (this->isIntersection == false || d < this->distance) {
      this->isIntersection = true;
      this->distance       = d;
      this->position       = p;
      this->face           = f;
    }
  }
};

DELEGATE_BIG4 (FaceIntersection)
DELEGATE3     (void, FaceIntersection, update, float, const glm::vec3&, LinkedFace)

GETTER (bool            , FaceIntersection, isIntersection)
GETTER (float           , FaceIntersection, distance)
GETTER (const glm::vec3&, FaceIntersection, position)
GETTER (LinkedFace      , FaceIntersection, face)

bool IntersectionUtil :: intersects ( const Sphere& sphere, const WingedMesh& mesh
                                    , const WingedVertex& vertex) {
  glm::vec3 v = vertex.vertex (mesh);
  return glm::distance (v,sphere.origin ()) <= sphere.radius ();
}

bool IntersectionUtil :: intersects ( const Sphere& sphere, const WingedMesh& mesh
                                    , const WingedEdge& edge) {
  glm::vec3 v1       = edge.vertex1 ()->vertex (mesh);
  glm::vec3 v2       = edge.vertex2 ()->vertex (mesh);
  glm::vec3 l        = v2 - v1;
  float     lsqr     = glm::dot (l,l);
  float     rsqr     = sphere.radius () * sphere.radius ();
  glm::vec3 v1o      = v1 - sphere.origin ();
  float     lv1o     = glm::dot (l, v1o);
  float     radicand = (lv1o * lv1o) - (lsqr * (glm::dot (v1o,v1o) - rsqr));

  if (radicand < 0.0f)
    return false;
  else {
    float d1 = (-lv1o + sqrt (radicand)) / lsqr;
    float d2 = (-lv1o - sqrt (radicand)) / lsqr;

    if ( (d1 < 0.0f && d2 < 0.0f) || (d1 > 1.0f && d2 > 1.0f) )
      return false;
    else 
      return true;
  }
}

bool IntersectionUtil :: intersects ( const Sphere& sphere, const WingedMesh& mesh
                                    , const WingedFace& face) {
  for (ADJACENT_VERTEX_ITERATOR (it,face)) {
    if (IntersectionUtil :: intersects (sphere, mesh, *it.vertex ()))
      return true;
  }
  for (ADJACENT_EDGE_ITERATOR (it,face)) {
    if (IntersectionUtil :: intersects (sphere, mesh, *it.edge ()))
      return true;
  }

  auto sameSide = [] ( const glm::vec3& p1,const glm::vec3& p2
                     , const glm::vec3& a ,const glm::vec3& b ) {
    glm::vec3 cross1 = glm::cross (b-a, p1-a);
    glm::vec3 cross2 = glm::cross (b-a, p2-a);
    return glm::dot (cross1,cross2) >= 0.0f;
  };

  glm::vec3 normal   = face.normal (mesh);
  glm::vec3 toOrigin = sphere.origin () - face.edge ()->vertex1 ()->vertex (mesh);
  float     d        = glm::dot (normal, toOrigin);
  glm::vec3 onPlane  = sphere.origin () - (d * normal);

  if (d < 0.0f || d > sphere.radius ())
    return false;

  for (ADJACENT_EDGE_ITERATOR (it,face)) {
    glm::vec3 other = it.edge ()->successor (face)->secondVertex (face)->vertex (mesh);
    assert (other != it.edge ()->vertex1 ()->vertex (mesh));
    assert (other != it.edge ()->vertex2 ()->vertex (mesh));
    if (! sameSide (other, onPlane, it.edge ()->vertex1 ()->vertex (mesh)
                                  , it.edge ()->vertex2 ()->vertex (mesh)))
      return false;
  }
  return true;
}
