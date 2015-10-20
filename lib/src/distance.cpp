/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "distance.hpp"
#include "primitive/cone.hpp"
#include "primitive/cylinder.hpp"
#include "primitive/sphere.hpp"
#include "util.hpp"

namespace {
  float distanceToCylinder ( const glm::vec3& center1, float radius, float length
                           , const glm::vec3& direction, const glm::vec3& point, float* tCyl )
  {
    const glm::vec3 toP = point - center1;
    const float x       = glm::dot (toP, direction);  
    const float y       = glm::sqrt (glm::dot (toP, toP) - (x*x));
    const float yr      = y - radius;
    const float xl      = x - length;
    const bool  insideR = yr <= 0.0f;

    if (x <= 0.0f) {
      return insideR ? glm::abs (x)
                     : glm::sqrt ((x * x) + (yr * yr));
    }
    else if (x >= length) {
      return insideR ? xl
                     : glm::sqrt ((xl * xl) + (yr * yr));
    }
    else {
      Util::setIfNotNull (tCyl, x);

      return insideR ? glm::max (-x, glm::max (yr, xl))
                     : yr;
    }
  }
}

float Distance::distance (const PrimSphere& sphere, const glm::vec3& point) {
    return glm::distance (sphere.center (), point) - sphere.radius ();
}

float Distance::distance (const PrimCylinder& cylinder, const glm::vec3& point, float* tCyl) {
  return distanceToCylinder ( cylinder.center1 (), cylinder.radius (), cylinder.length ()
                            , cylinder.direction (), point, tCyl );
}

float Distance::distance (const PrimCone& cone, const glm::vec3& point, float* tCone) {
  if (cone.isCylinder ()) {
    return distanceToCylinder ( cone.center1 (), cone.radius1 (), cone.length ()
                              , cone.direction (), point, tCone );
  }
  else {
    const glm::vec3 toP = point - cone.center1 ();
    const float x       = glm::dot (toP, cone.direction ());  
    const float y       = glm::sqrt (glm::dot (toP, toP) - (x * x));
    const float r1      = cone.radius1 ();
    const float r2      = cone.radius2 ();
    const float l       = cone.length ();

    if (x <= 0.0f) {
      return y <= r1 ? glm::abs (x)
                     : glm::sqrt ((x * x) + ((y - r1) * (y - r1)));
    }
    else if (x >= l && y <= r2) {
      return x - l;
    }
    else {
      const float xn    = (x * cone.cosAlpha ()) - ((y - r1) * cone.sinAlpha ());
      const float yn    = (x * cone.sinAlpha ()) + ((y - r1) * cone.cosAlpha ());
      const float delta = r1 - r2;
      const float s     = glm::sqrt ((l * l) + (delta * delta));

      if (xn <= 0.0f) {
        return glm::sqrt ((x * x) + ((y - r1) * (y - r1)));
      }
      else if (xn >= s) {
        if (x <= l) {
          assert (yn <= 0.0f);

          Util::setIfNotNull (tCone, x);
          return x - l;
        }
        else {
          assert (yn >= 0.0f);

          return glm::sqrt ((yn * yn) + ((xn - s) * (xn - s)));
        }
      }
      else if (yn >= 0.0f) {
        return yn;
      }
      else {
        Util::setIfNotNull (tCone, x);

        return glm::max (-x, glm::max (yn, x - l));
      }
    }
  }
}
