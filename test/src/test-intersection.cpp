/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "intersection.hpp"
#include "primitive/aabox.hpp"
#include "primitive/cone.hpp"
#include "primitive/cylinder.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "primitive/sphere.hpp"
#include "primitive/triangle.hpp"
#include "test-intersection.hpp"
#include "util.hpp"

void TestIntersection::test1 ()
{
  using IntersectionUtil::intersects;

  const glm::vec3 v1 (0.0f, 0.0f, 0.0f);
  const glm::vec3 v2 (2.0f, 0.0f, 0.0f);
  const glm::vec3 v3 (0.0f, 2.0f, 0.0f);

  PrimTriangle tri (v1, v2, v3);
  PrimSphere   sph (glm::vec3 (0.0f, 0.0f, 0.0f), 1.0f);
  PrimPlane    pln (glm::vec3 (0.0f, 0.0f, 0.0f), glm::vec3 (0.0f, 1.0f, 0.0f));
  PrimAABox    abx (glm::vec3 (0.0f, 0.0f, 0.0f), 1.0f);
  PrimCylinder cyl (glm::vec3 (0.0f, 0.0f, 0.0f), glm::vec3 (0.0f, 1.0f, 0.0f), 1.0f);
  PrimCone     cne (glm::vec3 (0.0f, 0.0f, 0.0f), 1.0f, glm::vec3 (0.0f, 1.0f, 0.0f), 0.5f);

  float t = 0.0f;

  assert (intersects (PrimRay (glm::vec3 (0.1f, 0.1f, 1.0f), glm::vec3 (0.0f, 0.0f, -1.0f)), tri,
                      false, nullptr));
  assert (intersects (PrimRay (glm::vec3 (0.1f, 0.1f, -1.0f), glm::vec3 (0.0f, 0.0f, -1.0f)), tri,
                      false, nullptr) == false);
  assert (intersects (PrimRay (true, glm::vec3 (0.1f, 0.1f, -1.0f), glm::vec3 (0.0f, 0.0f, -1.0f)),
                      tri, false, nullptr));

  assert (
    intersects (PrimRay (glm::vec3 (0.0f, 0.0f, 0.0f), glm::vec3 (0.0f, 0.0f, -1.0f)), sph, &t));
  assert (t > 0.0f);
  assert (
    intersects (PrimRay (glm::vec3 (0.0f, 0.0f, 2.0f), glm::vec3 (0.0f, 0.0f, -1.0f)), sph, &t));
  assert (t > 0.0f && t < 2.0f);
  assert (intersects (PrimRay (glm::vec3 (0.0f, 0.0f, -2.0f), glm::vec3 (0.0f, 0.0f, -1.0f)), sph,
                      &t) == false);
  assert (intersects (PrimRay (true, glm::vec3 (0.0f, 0.0f, -2.0f), glm::vec3 (0.0f, 0.0f, -1.0f)),
                      sph, &t));
  assert (t < 0.0f && t > -2.0f);

  assert (
    intersects (PrimRay (glm::vec3 (0.0f, 1.0f, 0.0f), glm::vec3 (0.0f, -1.0f, 0.0f)), pln, &t));
  assert (t > 0.0f);
  assert (
    intersects (PrimRay (glm::vec3 (0.0f, -1.0f, 0.0f), glm::vec3 (0.0f, 1.0f, 0.0f)), pln, &t));
  assert (t > 0.0f);
  assert (intersects (PrimRay (glm::vec3 (0.0f, 1.0f, 0.0f), glm::vec3 (0.0f, 1.0f, 0.0f)), pln,
                      &t) == false);
  assert (intersects (PrimRay (true, glm::vec3 (0.0f, 1.0f, 0.0f), glm::vec3 (0.0f, 1.0f, 0.0f)),
                      pln, &t));
  assert (t < 0.0f);

  assert (intersects (PrimRay (glm::vec3 (0.0f, 0.0f, 1.0f), glm::vec3 (0.0f, 0.0f, -1.0f)), abx,
                      nullptr));
  assert (intersects (PrimRay (glm::vec3 (0.0f, 0.0f, -1.0f), glm::vec3 (0.0f, 0.0f, 1.0f)), abx,
                      nullptr));
  assert (intersects (PrimRay (glm::vec3 (1.5f, 0.0f, 1.0f), glm::vec3 (0.0f, 0.0f, -1.0f)), abx,
                      nullptr) == false);
  assert (intersects (PrimRay (glm::vec3 (1.5f, 0.0f, -1.0f), glm::vec3 (0.0f, 0.0f, 1.0f)), abx,
                      nullptr) == false);
  assert (intersects (PrimRay (glm::vec3 (0.0f, 0.0f, -1.0f), glm::vec3 (0.0f, 0.0f, -1.0f)), abx,
                      nullptr) == false);
  assert (intersects (PrimRay (glm::vec3 (0.0f, 0.0f, 1.0f), glm::vec3 (0.0f, 0.0f, 1.0f)), abx,
                      nullptr) == false);
  assert (intersects (PrimRay (true, glm::vec3 (0.0f, 0.0f, -1.0f), glm::vec3 (0.0f, 0.0f, -1.0f)),
                      abx, nullptr));

  assert (intersects (PrimPlane (glm::vec3 (0.0f, 0.0f, 0.0f), glm::vec3 (0.0f, 1.0f, 0.0f)), abx));
  assert (intersects (PrimPlane (glm::vec3 (0.0f, 0.4f, 0.0f), glm::vec3 (0.0f, 1.0f, 0.0f)), abx));
  assert (intersects (PrimPlane (glm::vec3 (0.0f, 0.6f, 0.0f), glm::vec3 (0.0f, 1.0f, 0.0f)),
                      abx) == false);

  assert (intersects (PrimPlane (glm::vec3 (0.0f, 2.0f, 0.0f), glm::vec3 (0.0f, 1.0f, 0.0f)), tri));

  assert (intersects (cyl, glm::vec3 (0.0f, -0.1f, 0.0f)) == false);
  assert (intersects (cyl, glm::vec3 (0.0f, 1.1f, 0.0f)) == false);
  assert (intersects (cyl, glm::vec3 (1.1f, 0.1f, 0.0f)) == false);
  assert (intersects (cyl, glm::vec3 (1.0f, 0.0f, 0.0f)));
  assert (intersects (cyl, glm::vec3 (1.0f, 1.0f, 0.0f)));
  assert (intersects (cyl, glm::vec3 (0.9f, 0.1f, 0.0f)));
  assert (intersects (cyl, glm::vec3 (0.1f, 0.9f, 0.0f)));

  assert (intersects (cne, glm::vec3 (0.0f, -0.1f, 0.0f)) == false);
  assert (intersects (cne, glm::vec3 (1.0f, 0.1f, 0.0f)) == false);
  assert (intersects (cne, glm::vec3 (1.0f, 0.0f, 0.0f)));
  assert (intersects (cne, glm::vec3 (0.5f, 1.0f, 0.0f)));
  assert (intersects (cne, glm::vec3 (0.8f, 0.1f, 0.0f)));
  unused (t);
}

void TestIntersection::test2 ()
{
  Intersection i1, i2;

  i1.update (2.0f, glm::vec3 (2.0f), glm::vec3 (2.0f));
  i2.update (1.0f, glm::vec3 (1.0f), glm::vec3 (1.0f));

  Intersection::sort (i1, i2);

  assert (i1.distance () == 1.0f);
  assert (i1.position () == glm::vec3 (1.0f));
  assert (i1.normal () == glm::vec3 (1.0f));

  assert (i2.distance () == 2.0f);
  assert (i2.position () == glm::vec3 (2.0f));
  assert (i2.normal () == glm::vec3 (2.0f));
}
