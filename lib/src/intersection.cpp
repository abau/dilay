#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <limits>
#include "adjacent-iterator.hpp"
#include "intersection.hpp"
#include "primitive/aabox.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "primitive/sphere.hpp"
#include "primitive/triangle.hpp"
#include "util.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

struct Intersection :: Impl {
  bool      isIntersection;
  float     distance;
  glm::vec3 position;
  glm::vec3 normal;

  Impl () : isIntersection (false) {}

  bool update (float d, const glm::vec3& p, const glm::vec3& n) {
    if (this->isIntersection == false || d < this->distance) {
      this->isIntersection = true;
      this->distance       = d;
      this->position       = p;
      this->normal         = n;
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
DELEGATE3        (bool            , Intersection, update, float, const glm::vec3&, const glm::vec3&)
GETTER_CONST     (bool            , Intersection, isIntersection)
GETTER_CONST     (float           , Intersection, distance)
GETTER_CONST     (const glm::vec3&, Intersection, position)
GETTER_CONST     (const glm::vec3&, Intersection, normal)
DELEGATE2_STATIC (Intersection&   , Intersection, min, Intersection&, Intersection&)

namespace {
  template <typename T>
  void writeIfNotNull (T* ptr, T value) {
    if (ptr) {
      *ptr = value;
    }
  }
}

bool IntersectionUtil :: intersects (const PrimSphere& sphere, const glm::vec3& vec) {
  const glm::vec3 d = vec - sphere.center ();
  const float     r = sphere.radius ();
  return glm::dot (d,d) <= r * r;
}

bool IntersectionUtil :: intersects ( const PrimSphere& sphere, const WingedMesh& mesh
                                    , const WingedVertex& vertex) 
{
  return IntersectionUtil :: intersects (sphere, vertex.vector (mesh));
}

// see http://realtimecollisiondetection.net/blog/?p=103
bool IntersectionUtil :: intersects ( const PrimSphere& sphere, const WingedMesh& mesh
                                    , const WingedFace& face) {
  assert (face.isTriangle ());

  const glm::vec3 A    = face.firstVertex  ().vector (mesh) - sphere.center ();
  const glm::vec3 B    = face.secondVertex ().vector (mesh) - sphere.center ();
  const glm::vec3 C    = face.thirdVertex  ().vector (mesh) - sphere.center ();

  const float     rr   = sphere.radius () * sphere.radius ();
  const glm::vec3 V    = glm::cross (B-A, C-A);
  const float     d    = glm::dot   (A, V);
  const float     e    = glm::dot   (V, V);
  const bool      sep1 = d*d > rr * e;

  const float     aa   = glm::dot (A,A);
  const float     ab   = glm::dot (A,B);
  const float     ac   = glm::dot (A,C);
  const float     bb   = glm::dot (B,B);
  const float     bc   = glm::dot (B,C);
  const float     cc   = glm::dot (C,C);
  const bool      sep2 = (aa > rr) && (ab > aa) && (ac > aa);
  const bool      sep3 = (bb > rr) && (ab > bb) && (bc > bb);
  const bool      sep4 = (cc > rr) && (ac > cc) && (bc > cc);

  const glm::vec3 AB   = B - A;
  const glm::vec3 BC   = C - B;
  const glm::vec3 CA   = A - C;

  const float     d1   = ab - aa;
  const float     d2   = bc - bb;
  const float     d3   = ac - cc;
  const float     e1   = glm::dot (AB, AB);
  const float     e2   = glm::dot (BC, BC);
  const float     e3   = glm::dot (CA, CA);

  const glm::vec3 Q1   = (A * e1) - (d1 * AB);
  const glm::vec3 Q2   = (B * e2) - (d2 * BC);
  const glm::vec3 Q3   = (C * e3) - (d3 * CA);
  const glm::vec3 QC   = (C * e1) - Q1;
  const glm::vec3 QA   = (A * e2) - Q2;
  const glm::vec3 QB   = (B * e3) - Q3;

  const bool      sep5 = (glm::dot (Q1, Q1) > rr * e1 * e1) && (glm::dot (Q1, QC) > 0.0f);
  const bool      sep6 = (glm::dot (Q2, Q2) > rr * e2 * e2) && (glm::dot (Q2, QA) > 0.0f);
  const bool      sep7 = (glm::dot (Q3, Q3) > rr * e3 * e3) && (glm::dot (Q3, QB) > 0.0f);

  return (sep1 || sep2 || sep3 || sep4 || sep5 || sep6 || sep7) == false;
}

bool IntersectionUtil :: intersects ( const PrimSphere& sphere, const WingedMesh& mesh
                                    , const WingedEdge& edge ) 
{
  float t;
  const PrimRay ray (edge.vertex1Ref ().vector (mesh), edge.vertex2Ref ().vector (mesh)
                                                     - edge.vertex1Ref ().vector (mesh));
  return IntersectionUtil::intersects (ray, sphere, &t)
       ? t <= 1.0f
       : false;
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
  switch (n) {
    case 0:
      return false;

    case 1:
      if (s1 >= 0.0f || ray.isLine ()) {
        writeIfNotNull (t, s1);
        return true;
      }
      else {
        return false;
      }
    case 2: {
      const float sMin = glm::min (s1,s2);
      const float sMax = glm::max (s1,s2);

      if (sMin >= 0.0f) {
        writeIfNotNull (t, sMin);
        return true;
      }
      else if (sMax >= 0.0f || ray.isLine ()) {
        writeIfNotNull (t, sMax);
        return true;
      }
      return false;
    }
    default:
      std::abort ();
  }
}

bool IntersectionUtil :: intersects (const PrimRay& ray, const PrimPlane& plane, float* t) {
  const float d = glm::dot (ray.direction (), plane.normal ());

  if (glm::epsilonEqual (d, 0.0f, Util::epsilon ())) {
    return false;
  }
  const float s = glm::dot (plane.point () - ray.origin (), plane.normal ()) / d;

  if (s >= 0.0f || ray.isLine ()) {
    writeIfNotNull (t, s);
    return true;
  }
  else {
    return false;
  }
}

// see http://www.cs.virginia.edu/~gfx/Courses/2003/ImageSynthesis/papers/Acceleration/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
bool IntersectionUtil :: intersects ( const PrimRay& ray, const PrimTriangle& tri
                                    , glm::vec3* intersection) 
{
  const glm::vec3 e1 = tri.vertex2 () - tri.vertex1 ();
  const glm::vec3 e2 = tri.vertex3 () - tri.vertex1 ();
  const glm::vec3 s1 = glm::cross (ray.direction (), e2);
  const float det    = glm::dot   (s1, e1);
  const float invDet = 1.0f / det;

  if (glm::epsilonEqual (det, 0.0f, Util::epsilon ())) {
    return false;
  }
  const glm::vec3 d  = ray.origin () - tri.vertex1 ();
  const glm::vec3 s2 = glm::cross (d,e1);
  const float     b1 = glm::dot (d,s1) * invDet;
  const float     b2 = glm::dot (ray.direction (), s2) * invDet;
  const float     t  = glm::dot (e2, s2) * invDet;

  if (b1 < 0.0f || b2 < 0.0f || b1 + b2 > 1.0f || (t < 0.0f && ray.isLine () == false)) {
    return false;
  }
  else {
    writeIfNotNull (intersection, ray.pointAt (t));
    return true;
  }
}

bool IntersectionUtil :: intersects (const PrimRay& ray, const PrimAABox& box) {
  const glm::vec3 invDir  = glm::vec3 (1.0f) / ray.direction ();
  const glm::vec3 lowerTs = (box.minimum () - ray.origin ()) * invDir;
  const glm::vec3 upperTs = (box.maximum () - ray.origin ()) * invDir;
  const glm::vec3 min     = glm::min (lowerTs, upperTs);
  const glm::vec3 max     = glm::max (lowerTs, upperTs);

  const float tMin = glm::max ( glm::max (min.x, min.y), min.z );
  const float tMax = glm::min ( glm::min (max.x, max.y), max.z );

  return (tMax >= 0.0f || ray.isLine ()) && tMin <= tMax;
}
