/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "distance.hpp"
#include "primitive/cone-sphere.hpp"
#include "primitive/cone.hpp"
#include "primitive/cylinder.hpp"
#include "primitive/sphere.hpp"
#include "primitive/triangle.hpp"
#include "util.hpp"

namespace
{
  float distanceToCylinder (const glm::vec3& center1, float radius, float length,
                            const glm::vec3& direction, const glm::vec3& point)
  {
    const glm::vec3 toP = point - center1;
    const float     x = glm::dot (toP, direction);
    const float     ySqr = glm::dot (toP, toP) - (x * x);
    const float     y = Util::almostEqual (0.0f, ySqr) ? 0.0f : glm::sqrt (ySqr);
    const float     yr = y - radius;
    const float     xl = x - length;
    const bool      insideR = yr <= 0.0f;

    if (x <= 0.0f)
    {
      return insideR ? glm::abs (x) : glm::sqrt ((x * x) + (yr * yr));
    }
    else if (x >= length)
    {
      return insideR ? xl : glm::sqrt ((xl * xl) + (yr * yr));
    }
    else
    {
      return insideR ? glm::max (-x, glm::max (yr, xl)) : yr;
    }
  }
}

float Distance::distance (const PrimSphere& sphere, const glm::vec3& point)
{
  return glm::distance (sphere.center (), point) - sphere.radius ();
}

float Distance::distance (const PrimCylinder& cylinder, const glm::vec3& point)
{
  return distanceToCylinder (cylinder.center1 (), cylinder.radius (), cylinder.length (),
                             cylinder.direction (), point);
}

float Distance::distance (const PrimCone& cone, const glm::vec3& point)
{
  if (cone.isCylinder ())
  {
    return distanceToCylinder (cone.center1 (), cone.radius1 (), cone.length (), cone.direction (),
                               point);
  }
  else
  {
    const glm::vec3 toP = point - cone.center1 ();
    const float     x = glm::dot (toP, cone.direction ());
    const float     ySqr = glm::dot (toP, toP) - (x * x);
    const float     y = Util::almostEqual (0.0f, ySqr) ? 0.0f : glm::sqrt (ySqr);
    const float     r1 = cone.radius1 ();
    const float     r2 = cone.radius2 ();
    const float     l = cone.length ();

    if (x <= 0.0f)
    {
      return y <= r1 ? -x : glm::sqrt ((x * x) + ((y - r1) * (y - r1)));
    }
    else if (x >= l && y <= r2)
    {
      return x - l;
    }
    else
    {
      const float xn = (x * cone.cosAlpha ()) - ((y - r1) * cone.sinAlpha ());
      const float yn = (x * cone.sinAlpha ()) + ((y - r1) * cone.cosAlpha ());
      const float delta = r1 - r2;
      const float s = glm::sqrt ((l * l) + (delta * delta));

      if (xn <= 0.0f)
      {
        return glm::sqrt ((x * x) + ((y - r1) * (y - r1)));
      }
      else if (xn >= s)
      {
        if (x <= l)
        {
          assert (yn <= 0.0f);
          return x - l;
        }
        else
        {
          return glm::sqrt ((yn * yn) + ((xn - s) * (xn - s)));
        }
      }
      else if (yn >= 0.0f)
      {
        return yn;
      }
      else
      {
        return glm::max (-x, glm::max (yn, x - l));
      }
    }
  }
}

float Distance::distance (const PrimConeSphere& coneSphere, const glm::vec3& point)
{
  const glm::vec3 toP = point - coneSphere.sphere1 ().center ();
  const float     x = glm::dot (toP, coneSphere.direction ());
  const float     ySqr = glm::dot (toP, toP) - (x * x);
  const float     y = Util::almostEqual (0.0f, ySqr) ? 0.0f : glm::sqrt (ySqr);
  const float     r1 = coneSphere.sphere1 ().radius ();
  const float     r2 = coneSphere.sphere2 ().radius ();
  const float     l = coneSphere.length ();

  if (coneSphere.sameRadii ())
  {
    if (x <= 0.0f)
    {
      return glm::sqrt ((x * x) + (y * y)) - r1;
    }
    else if (x >= l)
    {
      return glm::sqrt (((x - l) * (x - l)) + (y * y)) - r1;
    }
    else
    {
      return y - r1;
    }
  }
  else if (coneSphere.hasCone ())
  {
    const float s = coneSphere.coneSideLength ();
    const float h1 = r1 * coneSphere.delta () / l;
    const float h2 = r2 * coneSphere.delta () / l;
    const float r1c = r1 * s / l;
    const float r2c = r2 * s / l;

    if (x <= 0.0f)
    {
      return glm::sqrt ((x * x) + (y * y)) - r1;
    }
    else if (x >= l + h2 && y <= r2c)
    {
      return glm::sqrt (((x - l) * (x - l)) + (y * y)) - r2;
    }
    else
    {
      const float xn = ((x - h1) * coneSphere.cosAlpha ()) - ((y - r1c) * coneSphere.sinAlpha ());
      const float yn = ((x - h1) * coneSphere.sinAlpha ()) + ((y - r1c) * coneSphere.cosAlpha ());

      if (xn <= 0.0f)
      {
        return glm::sqrt ((x * x) + (y * y)) - r1;
      }
      else if (xn >= s)
      {
        return glm::sqrt (((x - l) * (x - l)) + (y * y)) - r2;
      }
      else
      {
        return yn;
      }
    }
  }
  else
  {
    return glm::sqrt ((x * x) + (y * y)) - r1;
  }
}

// cf. https://www.geometrictools.com/Documentation/DistancePoint3Triangle3.pdf
float Distance::distance (const PrimTriangle& tri, const glm::vec3& point)
{
  const glm::vec3& P = point;
  const glm::vec3& B = tri.vertex1 ();
  const glm::vec3  E0 = tri.vertex2 () - tri.vertex1 ();
  const glm::vec3  E1 = tri.vertex3 () - tri.vertex1 ();
  const glm::vec3  D = B - P;

  const float a = glm::dot (E0, E0);
  const float b = glm::dot (E0, E1);
  const float c = glm::dot (E1, E1);
  const float d = glm::dot (E0, D);
  const float e = glm::dot (E1, D);

  float       s = (b * e) - (c * d);
  float       t = (b * d) - (a * e);
  const float det = (a * c) - (b * b);

  if (s + t <= det)
  {
    if (s < 0.0f)
    {
      if (t < 0.0f)
      {
        // region 4
        if (d < 0.0f)
        {
          t = 0.0f;
          s = -d > a ? 1.0f : -d / a;
        }
        else
        {
          s = 0.0f;
          if (e >= 0.0f)
          {
            t = 0.0f;
          }
          else if (-e >= c)
          {
            t = 1.0f;
          }
          else
          {
            t = -e / c;
          }
        }
      }
      else
      {
        // region 3
        s = 0.0f;
        if (e >= 0.0f)
        {
          t = 0.0f;
        }
        else if (-e >= c)
        {
          t = 1.0f;
        }
        else
        {
          t = -e / c;
        }
      }
    }
    else if (t < 0.0f)
    {
      // region 5
      t = 0.0f;
      if (d >= 0.0f)
      {
        s = 0.0f;
      }
      else if (-d >= a)
      {
        s = 1.0f;
      }
      else
      {
        s = -d / a;
      }
    }
    else
    {
      // region 0
      s /= det;
      t /= det;
    }
  }
  else
  {
    if (s < 0.0f)
    {
      // region 2
      const float tmp0 = b + d;
      const float tmp1 = c + e;
      if (tmp1 > tmp0)
      {
        const float numer = tmp1 - tmp0;
        const float denom = a - (2.0f * b) + c;
        s = numer >= denom ? 1.0f : numer / denom;
        t = 1.0f - s;
      }
      else
      {
        s = 0.0f;
        if (tmp1 <= 0.0f)
        {
          t = 1.0f;
        }
        else if (e >= 0.0f)
        {
          t = 0.0f;
        }
        else
        {
          t = -e / c;
        }
      }
    }
    else if (t < 0.0f)
    {
      // region 6
      const float tmp0 = b + e;
      const float tmp1 = a + d;
      if (tmp1 > tmp0)
      {
        const float numer = tmp1 - tmp0;
        const float denom = a - (2.0f * b) + c;
        t = numer >= denom ? 1.0f : numer / denom;
        s = 1.0f - t;
      }
      else
      {
        t = 0.0f;
        if (tmp1 <= 0.0f)
        {
          s = 1.0f;
        }
        else if (d >= 0.0f)
        {
          s = 0.0f;
        }
        else
        {
          s = -d / a;
        }
      }
    }
    else
    {
      // region 1
      const float numer = (c + e) - (b + d);
      if (numer <= 0.0f)
      {
        s = 0.0f;
      }
      else
      {
        const float denom = a - (2.0f * b) + c;
        s = numer >= denom ? 1.0f : numer / denom;
      }
      t = 1.0f - s;
    }
  }
  return glm::distance (point, B + (s * E0) + (t * E1));
}
