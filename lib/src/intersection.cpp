/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "intersection.hpp"
#include "primitive/aabox.hpp"
#include "primitive/cone.hpp"
#include "primitive/cylinder.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "primitive/sphere.hpp"
#include "primitive/triangle.hpp"
#include "util.hpp"

struct Intersection::Impl
{
  bool      isIntersection;
  float     distance;
  glm::vec3 position;
  glm::vec3 normal;

  Impl ()
    : isIntersection (false)
  {
  }

  void reset ()
  {
    this->isIntersection = false;
  }

  bool update (float d, const glm::vec3& p, const glm::vec3& n)
  {
    if (this->isIntersection == false || d < this->distance)
    {
      this->isIntersection = true;
      this->distance = d;
      this->position = p;
      this->normal = n;
      return true;
    }
    return false;
  }

  static Intersection& min (Intersection& a, Intersection& b)
  {
    if (a.isIntersection () && (b.isIntersection () == false || a.distance () < b.distance ()))
    {
      return a;
    }
    else
    {
      return b;
    }
  }
};

DELEGATE_BIG6 (Intersection)
DELEGATE (void, Intersection, reset)
GETTER_CONST (bool, Intersection, isIntersection)
GETTER_CONST (float, Intersection, distance)
GETTER_CONST (const glm::vec3&, Intersection, position)
GETTER_CONST (const glm::vec3&, Intersection, normal)
DELEGATE3 (bool, Intersection, update, float, const glm::vec3&, const glm::vec3&)
DELEGATE2_STATIC (Intersection&, Intersection, min, Intersection&, Intersection&)

namespace
{
  bool intersectsQuadric (float a, float b, float c, bool alongLine, float* t)
  {
    float              s1, s2;
    const unsigned int n = Util::solveQuadraticEq (a, b, c, s1, s2);

    switch (n)
    {
      case 0:
        return false;

      case 1:
        if (s1 >= 0.0f || alongLine)
        {
          Util::setIfNotNull (t, s1);
          return true;
        }
        else
        {
          return false;
        }

      case 2:
      {
        const float sMin = glm::min (s1, s2);
        const float sMax = glm::max (s1, s2);

        if (sMin >= 0.0f)
        {
          Util::setIfNotNull (t, sMin);
          return true;
        }
        else if (sMax >= 0.0f || alongLine)
        {
          Util::setIfNotNull (t, sMax);
          return true;
        }
        return false;
      }
      default:
        DILAY_IMPOSSIBLE
    }
  }
}

// see http://realtimecollisiondetection.net/blog/?p=103
bool IntersectionUtil::intersects (const PrimSphere& sphere, const PrimTriangle& tri)
{
  const glm::vec3 A = tri.vertex1 () - sphere.center ();
  const glm::vec3 B = tri.vertex2 () - sphere.center ();
  const glm::vec3 C = tri.vertex3 () - sphere.center ();

  const float     rr = sphere.radius () * sphere.radius ();
  const glm::vec3 V = glm::cross (B - A, C - A);
  const float     d = glm::dot (A, V);
  const float     e = glm::dot (V, V);
  const bool      sep1 = d * d > rr * e;

  const float aa = glm::dot (A, A);
  const float ab = glm::dot (A, B);
  const float ac = glm::dot (A, C);
  const float bb = glm::dot (B, B);
  const float bc = glm::dot (B, C);
  const float cc = glm::dot (C, C);
  const bool  sep2 = (aa > rr) && (ab > aa) && (ac > aa);
  const bool  sep3 = (bb > rr) && (ab > bb) && (bc > bb);
  const bool  sep4 = (cc > rr) && (ac > cc) && (bc > cc);

  const glm::vec3 AB = B - A;
  const glm::vec3 BC = C - B;
  const glm::vec3 CA = A - C;

  const float d1 = ab - aa;
  const float d2 = bc - bb;
  const float d3 = ac - cc;
  const float e1 = glm::dot (AB, AB);
  const float e2 = glm::dot (BC, BC);
  const float e3 = glm::dot (CA, CA);

  const glm::vec3 Q1 = (A * e1) - (d1 * AB);
  const glm::vec3 Q2 = (B * e2) - (d2 * BC);
  const glm::vec3 Q3 = (C * e3) - (d3 * CA);
  const glm::vec3 QC = (C * e1) - Q1;
  const glm::vec3 QA = (A * e2) - Q2;
  const glm::vec3 QB = (B * e3) - Q3;

  const bool sep5 = (glm::dot (Q1, Q1) > rr * e1 * e1) && (glm::dot (Q1, QC) > 0.0f);
  const bool sep6 = (glm::dot (Q2, Q2) > rr * e2 * e2) && (glm::dot (Q2, QA) > 0.0f);
  const bool sep7 = (glm::dot (Q3, Q3) > rr * e3 * e3) && (glm::dot (Q3, QB) > 0.0f);

  return (sep1 || sep2 || sep3 || sep4 || sep5 || sep6 || sep7) == false;
}

bool IntersectionUtil::intersects (const PrimSphere& sphere, const PrimAABox& box)
{
  const glm::vec3  c = sphere.center ();
  const glm::vec3& min = box.minimum ();
  const glm::vec3& max = box.maximum ();
  float            d = 0.0f;

  for (unsigned int i = 0; i < 3; i++)
  {
    if (c[i] < min[i])
    {
      const float s = c[i] - min[i];
      d += s * s;
    }
    else if (c[i] > max[i])
    {
      const float s = c[i] - max[i];
      d += s * s;
    }
  }
  return d <= sphere.radius () * sphere.radius ();
}

bool IntersectionUtil::intersects (const PrimSphere& sphere1, const PrimSphere& sphere2)
{
  return glm::distance2 (sphere1.center (), sphere2.center ()) <
         ((sphere1.radius () + sphere2.radius ()) * (sphere1.radius () + sphere2.radius ()));
}

bool IntersectionUtil::intersects (const PrimRay& ray, const PrimSphere& sphere, float* t)
{
  const glm::vec3& dir = ray.direction ();
  const glm::vec3& origin = ray.origin () - sphere.center ();
  const float      radius2 = sphere.radius () * sphere.radius ();

  return intersectsQuadric (glm::dot (dir, dir), 2.0f * glm::dot (dir, origin),
                            glm::dot (origin, origin) - radius2, ray.isLine (), t);
}

bool IntersectionUtil::intersects (const PrimRay& ray, const PrimPlane& plane, float* t)
{
  const float d = glm::dot (ray.direction (), plane.normal ());

  if (Util::almostEqual (d, 0.0f))
  {
    return false;
  }
  const float s = glm::dot (plane.point () - ray.origin (), plane.normal ()) / d;

  if (s >= 0.0f || ray.isLine ())
  {
    Util::setIfNotNull (t, s);
    return true;
  }
  else
  {
    return false;
  }
}

// see
// http://www.cs.virginia.edu/~gfx/Courses/2003/ImageSynthesis/papers/Acceleration/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
bool IntersectionUtil::intersects (const PrimRay& ray, const PrimTriangle& tri, float* t)
{
  const float dot = glm::dot (ray.direction (), tri.normal ());

  if (dot > -Util::epsilon ())
  {
    return false;
  }
  const glm::vec3 e1 = tri.vertex2 () - tri.vertex1 ();
  const glm::vec3 e2 = tri.vertex3 () - tri.vertex1 ();
  const glm::vec3 s1 = glm::cross (ray.direction (), e2);
  const float     invDet = 1.0f / glm::dot (s1, e1);
  const glm::vec3 d = ray.origin () - tri.vertex1 ();
  const glm::vec3 s2 = glm::cross (d, e1);
  const float     b1 = glm::dot (d, s1) * invDet;
  const float     b2 = glm::dot (ray.direction (), s2) * invDet;
  const float     tRay = glm::dot (e2, s2) * invDet;

  if (b1 < 0.0f || b2 < 0.0f || b1 + b2 > 1.0f || (tRay < 0.0f && ray.isLine () == false))
  {
    return false;
  }
  else
  {
    Util::setIfNotNull (t, tRay);
    return true;
  }
}

bool IntersectionUtil::intersects (const PrimRay& ray, const PrimAABox& box)
{
  const glm::vec3 invDir = glm::vec3 (1.0f) / ray.direction ();
  const glm::vec3 lowerTs = (box.minimum () - ray.origin ()) * invDir;
  const glm::vec3 upperTs = (box.maximum () - ray.origin ()) * invDir;
  const glm::vec3 min = glm::min (lowerTs, upperTs);
  const glm::vec3 max = glm::max (lowerTs, upperTs);

  const float tMin = glm::max (glm::max (min.x, min.y), min.z);
  const float tMax = glm::min (glm::min (max.x, max.y), max.z);

  return (tMax >= 0.0f || ray.isLine ()) && tMin <= tMax;
}

bool IntersectionUtil::intersects (const PrimRay& ray, const PrimCylinder& cylinder, float* tRay,
                                   float* tCyl)
{
  const glm::vec3& dir = ray.direction ();
  const glm::vec3& cylDir = cylinder.direction ();
  const glm::vec3  offset = ray.origin () - cylinder.center1 ();
  const glm::vec3  d = dir - (cylDir * glm::dot (cylDir, dir));
  const glm::vec3  c = offset - (cylDir * glm::dot (cylDir, offset));
  const float      radius2 = cylinder.radius () * cylinder.radius ();
  float            tmpRay;

  if (intersectsQuadric (glm::dot (d, d), 2.0f * glm::dot (c, d), glm::dot (c, c) - radius2,
                         ray.isLine (), &tmpRay))
  {
    const glm::vec3 p = ray.pointAt (tmpRay);
    const float     tmpCyl = glm::dot (cylDir, p - cylinder.center1 ());

    if (tmpCyl >= 0.0f && tmpCyl <= cylinder.length ())
    {
      Util::setIfNotNull (tRay, tmpRay);
      Util::setIfNotNull (tCyl, tmpCyl);
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}

bool IntersectionUtil::intersects (const PrimRay& ray, const PrimCone& cone, float* tRay,
                                   float* tCone)
{
  if (cone.isCylinder ())
  {
    return IntersectionUtil::intersects (ray, PrimCylinder (cone), tRay, tCone);
  }
  else
  {
    const glm::vec3& dir = ray.direction ();
    const glm::vec3& coneDir = cone.direction ();
    const glm::vec3  offset = ray.origin () - cone.apex ();
    const float      dotD = glm::dot (coneDir, dir);
    const float      dotO = glm::dot (coneDir, offset);
    const glm::vec3  d = dir - (coneDir * dotD);
    const glm::vec3  c = offset - (coneDir * glm::dot (coneDir, offset));
    const float      sinSqr = cone.sinAlpha () * cone.sinAlpha ();
    const float      cosSqr = cone.cosAlpha () * cone.cosAlpha ();
    float            tmpRay;

    if (intersectsQuadric ((cosSqr * glm::dot (d, d)) - (sinSqr * dotD * dotD),
                           (2.0f * cosSqr * glm::dot (c, d)) - (2.0f * sinSqr * dotD * dotO),
                           (cosSqr * glm::dot (c, c)) - (sinSqr * dotO * dotO), ray.isLine (),
                           &tmpRay))
    {
      const glm::vec3 p = ray.pointAt (tmpRay);
      const float     tmpCone = glm::dot (coneDir, p - cone.center1 ());

      if (tmpCone >= 0.0f && tmpCone <= cone.length ())
      {
        Util::setIfNotNull (tRay, tmpRay);
        Util::setIfNotNull (tCone, tmpCone);
        return true;
      }
      else
      {
        return false;
      }
    }
    else
    {
      return false;
    }
  }
}

bool IntersectionUtil::intersects (const PrimPlane& plane, const PrimAABox& box)
{
  const glm::vec3& max = box.maximum ();
  const glm::vec3& min = box.minimum ();
  const glm::vec3  p =
    glm::vec3 (plane.normal ().x >= 0.0f ? max.x : min.x, plane.normal ().y >= 0.0f ? max.y : min.y,
               plane.normal ().z >= 0.0f ? max.z : min.z);
  const glm::vec3 n =
    glm::vec3 (plane.normal ().x >= 0.0f ? min.x : max.x, plane.normal ().y >= 0.0f ? min.y : max.y,
               plane.normal ().z >= 0.0f ? min.z : max.z);

  return plane.distance (p) * plane.distance (n) < 0.0f;
}

bool IntersectionUtil::intersects (const PrimPlane& plane, const PrimTriangle& tri)
{
  const float d1 = plane.distance (tri.vertex1 ());
  const float d2 = plane.distance (tri.vertex2 ());
  const float d3 = plane.distance (tri.vertex3 ());

  const bool oneLess = d1 < Util::epsilon () || d2 < Util::epsilon () || d3 < Util::epsilon ();
  const bool oneGreater =
    d1 > -Util::epsilon () || d2 > -Util::epsilon () || d3 > -Util::epsilon ();

  return oneLess && oneGreater;
}

bool IntersectionUtil::intersects (const PrimCylinder& cylinder, const glm::vec3& point)
{
  const glm::vec3 d = point - cylinder.center1 ();
  const float     dot = glm::dot (cylinder.direction (), d);
  const glm::vec3 t = d - (cylinder.direction () * dot);

  return dot >= 0.0f && dot <= cylinder.length () &&
         glm::dot (t, t) <= cylinder.radius () * cylinder.radius ();
}

bool IntersectionUtil::intersects (const PrimCone& cone, const glm::vec3& point)
{
  if (cone.isCylinder ())
  {
    return IntersectionUtil::intersects (PrimCylinder (cone), point);
  }
  else
  {
    const glm::vec3 d = point - cone.center1 ();
    const float     dot = glm::dot (cone.direction (), d);
    const glm::vec3 t = d - (cone.direction () * dot);
    const float     r = glm::mix (cone.radius1 (), cone.radius2 (), dot / cone.length ());

    return dot >= 0.0f && dot <= cone.length () && glm::dot (t, t) <= r * r;
  }
}

bool IntersectionUtil::intersects (const PrimAABox& a, const PrimAABox& b)
{
  const bool overlap1 = glm::all (glm::greaterThanEqual (a.minimum (), b.maximum ()));
  const bool overlap2 = glm::all (glm::greaterThanEqual (b.minimum (), a.maximum ()));
  const bool contain1 = glm::all (glm::greaterThan (a.maximum (), b.maximum ())) &&
                        glm::all (glm::lessThan (a.minimum (), b.minimum ()));
  const bool contain2 = glm::all (glm::greaterThan (b.maximum (), a.maximum ())) &&
                        glm::all (glm::lessThan (b.minimum (), a.minimum ()));

  return overlap1 || overlap2 || contain1 || contain2;
}

// http://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/code/tribox_tam.pdf
bool IntersectionUtil::intersects (const PrimAABox& box, const PrimTriangle& tri)
{
  const glm::vec3 v0 = tri.vertex1 () - box.center ();
  const glm::vec3 v1 = tri.vertex2 () - box.center ();
  const glm::vec3 v2 = tri.vertex3 () - box.center ();
  const glm::vec3 e0 = v1 - v0;
  const glm::vec3 e1 = v2 - v1;
  const glm::vec3 e2 = v0 - v2;
  const glm::vec3 hw = box.halfWidth ();
  const glm::vec3 x = glm::vec3 (1.0f, 0.0f, 0.0f);
  const glm::vec3 y = glm::vec3 (0.0f, 1.0f, 0.0f);
  const glm::vec3 z = glm::vec3 (0.0f, 0.0f, 1.0f);

  auto sepAxis = [&v0, &v1, &v2, &hw](const glm::vec3& a) -> bool {
    const float radius = glm::dot (hw, glm::abs (a));
    const float p0 = glm::dot (a, v0);
    const float p1 = glm::dot (a, v1);
    const float p2 = glm::dot (a, v2);
    const float min = glm::min (p0, glm::min (p1, p2));
    const float max = glm::max (p0, glm::max (p1, p2));

    return min > radius || max < -radius;
  };

  const bool sep1 = glm::min (v0.x, glm::min (v1.x, v2.x)) > hw.x;
  const bool sep2 = glm::max (v0.x, glm::max (v1.x, v2.x)) < -hw.x;

  const bool sep3 = glm::min (v0.y, glm::min (v1.y, v2.y)) > hw.y;
  const bool sep4 = glm::max (v0.y, glm::max (v1.y, v2.y)) < -hw.y;

  const bool sep5 = glm::min (v0.z, glm::min (v1.z, v2.z)) > hw.z;
  const bool sep6 = glm::max (v0.z, glm::max (v1.z, v2.z)) < -hw.z;

  if (sep1 || sep2 || sep3 || sep4 || sep5 || sep6)
  {
    return false;
  }

  const bool sep7 = sepAxis (glm::cross (x, e0));
  const bool sep8 = sepAxis (glm::cross (x, e1));
  const bool sep9 = sepAxis (glm::cross (x, e2));

  const bool sep10 = sepAxis (glm::cross (y, e0));
  const bool sep11 = sepAxis (glm::cross (y, e1));
  const bool sep12 = sepAxis (glm::cross (y, e2));

  const bool sep13 = sepAxis (glm::cross (z, e0));
  const bool sep14 = sepAxis (glm::cross (z, e1));
  const bool sep15 = sepAxis (glm::cross (z, e2));

  if (sep7 || sep8 || sep9 || sep10 || sep11 || sep12 || sep13 || sep14 || sep15)
  {
    return false;
  }
  else
  {
    return IntersectionUtil::intersects (PrimPlane (tri.vertex1 (), tri.normal ()), box);
  }
}
