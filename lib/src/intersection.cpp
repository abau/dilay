#include <limits>
#include <glm/glm.hpp>
#include "intersection.hpp"
#include "winged/vertex.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "primitive/sphere.hpp"
#include "adjacent-iterator.hpp"
#include "primitive/ray.hpp"
#include "util.hpp"
#include "primitive/plane.hpp"
#include "primitive/triangle.hpp"
#include "primitive/aabox.hpp"

struct Intersection :: Impl {
  bool      isIntersection;
  float     distance;
  glm::vec3 position;

  Impl () : isIntersection (false) {}

  bool update (float d, const glm::vec3& p) {
    if (this->isIntersection == false || d < this->distance) {
      this->isIntersection = true;
      this->distance       = d;
      this->position       = p;
      return true;
    }
    return false;
  }

  void reset () {
    this->isIntersection = false;
  }

  static Intersection& min (Intersection& a, Intersection& b) {
    if (a.isIntersection () && (b.isIntersection () == false || a.distance () < b.distance ())) {
      return a;
    }
    else {
      return b;
    }
  }
};

DELEGATE_BIG6    (Intersection)
DELEGATE2        (bool            , Intersection, update, float, const glm::vec3&)
GETTER_CONST     (bool            , Intersection, isIntersection)
GETTER_CONST     (float           , Intersection, distance)
GETTER_CONST     (const glm::vec3&, Intersection, position)
DELEGATE2_STATIC (Intersection&   , Intersection, min, Intersection&, Intersection&)

bool IntersectionUtil :: intersects (const PrimSphere& sphere, const glm::vec3& vec) {
  return glm::distance (vec,sphere.center ()) <= sphere.radius ();
}

bool IntersectionUtil :: intersects ( const PrimSphere& sphere, const WingedMesh& mesh
                                    , const WingedVertex& vertex) {
  return IntersectionUtil :: intersects (sphere, vertex.vertex (mesh));
}

bool IntersectionUtil :: intersects ( const PrimSphere& sphere, const WingedMesh& mesh
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

bool IntersectionUtil :: intersects ( const PrimSphere& sphere, const WingedMesh& mesh
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

bool IntersectionUtil :: intersects (const PrimSphere& sphere, const PrimAABox& box) {
  const glm::vec3  c   = sphere.center ();
  const glm::vec3& min = box.minimum ();
  const glm::vec3& max = box.maximum ();
  float            d   = 0.0f;

  for (unsigned int i = 0; i < 3; i++) {
    if (c[i] < min[i]) {
      const float s = c[i] - min[i];
      d += s * s;
    }
    else if (c[i] > max[i]) {
      const float s = c[i] - max[i];
      d += s * s;
    }
  }
  return d <= sphere.radius () * sphere.radius ();
}

bool IntersectionUtil :: intersects (const PrimRay& ray, const PrimSphere& sphere, float* t) {
  float s1,s2;
  const glm::vec3& d  = ray.direction ();
  const glm::vec3& v  = ray.origin () - sphere.center ();
  const float      r2 = sphere.radius () * sphere.radius ();

  const unsigned int n = Util :: solveQuadraticEq ( glm::dot (d,d)
                                                  , 2.0f * glm::dot (d,v)
                                                  , glm::dot (v, v) - r2
                                                  , s1, s2);
  if (n == 0)
    return false;
  else if (n == 1) {
    if (t) {
      *t = s1;
    }
    return true;
  }
  else {
    if (t) {
      *t = glm::min (s1,s2);
    }
    return true;
  }
}

bool IntersectionUtil :: intersects (const PrimRay& ray, const PrimPlane& plane, float* t) {
  const float d = glm::dot (ray.direction (), plane.normal ());

  if (d > - std::numeric_limits<float>::epsilon ()) {
    return false;
  }
  if (t) {
    *t = glm::dot (plane.point () - ray.origin (), plane.normal ()) / d;
  }
  return true;
}

bool IntersectionUtil :: intersects ( const PrimRay& ray, const PrimTriangle& tri
                                    , glm::vec3* intersection) {
  glm::vec3 e1 = tri.edge1 ();
  glm::vec3 e2 = tri.edge2 ();

  glm::vec3 s1  = glm::cross (ray.direction (), e2);
  float divisor = glm::dot   (s1, e1);

  if (divisor < std::numeric_limits<float>::epsilon ()) 
    return false;

  float     invDivisor = 1.0f / divisor;
  glm::vec3 d          = ray.origin () - tri.vertex1 ();
  glm::vec3 s2         = glm::cross (d,e1);
  float     b1         = glm::dot (d,s1)                 * invDivisor;
  float     b2         = glm::dot (ray.direction (), s2) * invDivisor;
  float     t          = glm::dot (e2, s2)               * invDivisor;

  if (b1 < 0.0f || b2 < 0.0f || b1 + b2 > 1.0f || t < 0.0f) {
    return false;
  }
  else {
    if (intersection) {
      *intersection = ray.pointAt (t);
    }
    return true;
  }
}

bool IntersectionUtil :: intersects (const PrimRay& ray, const PrimAABox& box) {
  glm::vec3 invDir  = glm::vec3 (1.0f) / ray.direction ();
  glm::vec3 lowerTs = (box.minimum () - ray.origin ()) * invDir;
  glm::vec3 upperTs = (box.maximum () - ray.origin ()) * invDir;
  glm::vec3 min     = glm::min (lowerTs, upperTs);
  glm::vec3 max     = glm::max (lowerTs, upperTs);

  float tMin = glm::max ( glm::max (min.x, min.y), min.z );
  float tMax = glm::min ( glm::min (max.x, max.y), max.z );

  if (tMax < 0.0f || tMin > tMax)
    return false;
  else
    return true;
}
