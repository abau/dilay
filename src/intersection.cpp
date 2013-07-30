#include <cmath>
#include <glm/glm.hpp>
#include "intersection.hpp"
#include "winged-vertex.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "macro.hpp"
#include "sphere.hpp"
#include "adjacent-iterator.hpp"
#include "ray.hpp"
#include "util.hpp"
#include "octree.hpp"

struct FaceIntersection::Impl {
  bool        isIntersection;
  float       distance;
  glm::vec3   position;
  WingedFace* _face;

  Impl () : isIntersection (false) {}

  void update (float d, const glm::vec3& p, WingedFace& f) {
    if (this->isIntersection == false || d < this->distance) {
      this->isIntersection = true;
      this->distance       = d;
      this->position       = p;
      this->_face          = &f;
    }
  }

  void reset () {
    this->isIntersection = false;
  }

  WingedFace& face () const {
    assert (this->isIntersection);
    return *this->_face;
  }
};

DELEGATE_BIG4  (FaceIntersection)
DELEGATE3      (void            , FaceIntersection, update, float, const glm::vec3&, WingedFace&)
DELEGATE       (void            , FaceIntersection, reset)
DELEGATE_CONST (WingedFace&     , FaceIntersection, face)
GETTER         (bool            , FaceIntersection, isIntersection)
GETTER         (float           , FaceIntersection, distance)
GETTER         (const glm::vec3&, FaceIntersection, position)

bool IntersectionUtil :: intersects (const Sphere& sphere, const glm::vec3& vec) {
  return glm::distance (vec,sphere.center ()) <= sphere.radius ();
}

bool IntersectionUtil :: intersects ( const Sphere& sphere, const WingedMesh& mesh
                                    , const WingedVertex& vertex) {
  return IntersectionUtil :: intersects (sphere, vertex.vertex (mesh));
}

bool IntersectionUtil :: intersects ( const Sphere& sphere, const WingedMesh& mesh
                                    , const WingedEdge& edge) {
  glm::vec3 v1       = edge.vertex1 ()->vertex (mesh);
  glm::vec3 v2       = edge.vertex2 ()->vertex (mesh);
  glm::vec3 l        = v2 - v1;
  float     lsqr     = glm::dot (l,l);
  float     rsqr     = sphere.radius () * sphere.radius ();
  glm::vec3 v1o      = v1 - sphere.center ();
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
    if (IntersectionUtil :: intersects (sphere, mesh, it.element ()))
      return true;
  }
  for (ADJACENT_EDGE_ITERATOR (it,face)) {
    if (IntersectionUtil :: intersects (sphere, mesh, it.element ()))
      return true;
  }

  auto sameSide = [] ( const glm::vec3& p1,const glm::vec3& p2
                     , const glm::vec3& a ,const glm::vec3& b ) {
    glm::vec3 cross1 = glm::cross (b-a, p1-a);
    glm::vec3 cross2 = glm::cross (b-a, p2-a);
    return glm::dot (cross1,cross2) >= 0.0f;
  };

  glm::vec3 normal   = face.normal (mesh);
  glm::vec3 toOrigin = sphere.center () - face.edge ()->vertex1 ()->vertex (mesh);
  float     d        = glm::dot (normal, toOrigin);
  glm::vec3 onPlane  = sphere.center () - (d * normal);

  if (d < 0.0f || d > sphere.radius ())
    return false;

  for (ADJACENT_EDGE_ITERATOR (it,face)) {
    glm::vec3 other = it.element ().successorRef (face)
                                   .secondVertexRef (face)
                                   .vertex (mesh);
    assert (other != it.element ().vertex1Ref ().vertex (mesh));
    assert (other != it.element ().vertex2Ref ().vertex (mesh));
    if (! sameSide (other, onPlane, it.element ().vertex1Ref ().vertex (mesh)
                                  , it.element ().vertex2Ref ().vertex (mesh)))
      return false;
  }
  return true;
}

bool IntersectionUtil :: intersects (const Sphere& sphere, const OctreeNode& node) {
  const float     w = node.looseWidth () * 0.5f;
  const glm::vec3 b = node.center ();
  const glm::vec3 c = sphere.center ();
  float         s,d = 0.0f;

  for (unsigned int i = 0; i < 3; i++) {
    if (c[i] < b[i] - w) {
      s  = c[i] - b[i] + w;
      d += s * s;
    }
    else if (c[i] > b[i] + w) {
      s  = c[i] - b[i] - w;
      d += s * s;
    }
  }
  return d <= sphere.radius () * sphere.radius ();
}

bool IntersectionUtil :: intersects (const Ray& ray, const Sphere& sphere, float& t) {
  float s1,s2;
  const glm::vec3& d  = ray.direction ();
  const glm::vec3& v  = ray.origin () - sphere.center ();
  float            r2 = sphere.radius () * sphere.radius ();

  unsigned int n = Util :: solveQuadraticEq ( glm::dot (d,d)
                                            , 2.0f * glm::dot (d,v)
                                            , glm::dot (v, v) - r2
                                            , s1, s2);
  if (n == 0)
    return false;
  else if (n == 1) {
    t = s1;
    return true;
  }
  else {
    t = glm::min (s1,s2);
    return true;
  }
}
