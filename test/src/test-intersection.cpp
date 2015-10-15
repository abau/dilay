/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
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

void TestIntersection::test () {
  using IntersectionUtil::intersects;

  PrimTriangle tri ( glm::vec3 (0.0f, 0.0f, 0.0f)
                   , glm::vec3 (2.0f, 0.0f, 0.0f)
                   , glm::vec3 (0.0f, 2.0f, 0.0f) );
  PrimSphere   sph ( glm::vec3 (0.0f, 0.0f, 0.0f), 1.0f );
  PrimPlane    pln ( glm::vec3 (0.0f, 0.0f, 0.0f)
                   , glm::vec3 (0.0f, 1.0f, 0.0f) );
  PrimAABox    abx ( glm::vec3 (0.0f, 0.0f, 0.0f), 1.0f );
  PrimCylinder cyl ( glm::vec3 (0.0f, 0.0f, 0.0f)
                   , glm::vec3 (0.0f, 1.0f, 0.0f), 1.0f );
  PrimCone     cne ( glm::vec3 (0.0f, 0.0f, 0.0f), 1.0f
                   , glm::vec3 (0.0f, 1.0f, 0.0f), 0.5f );

  float t = 0.0f;

  assert (intersects (PrimRay ( glm::vec3 (0.0f, 0.0f, 1.0f)
                              , glm::vec3 (0.0f, 0.0f,-1.0f) ), tri, nullptr));
  assert (intersects (PrimRay ( glm::vec3 (0.0f, 0.0f,-1.0f)
                              , glm::vec3 (0.0f, 0.0f,-1.0f) ), tri, nullptr) == false);
  assert (intersects (PrimRay ( true
                              , glm::vec3 (0.0f, 0.0f,-1.0f)
                              , glm::vec3 (0.0f, 0.0f,-1.0f) ), tri, nullptr));

  assert (intersects (PrimRay ( glm::vec3 (0.0f, 0.0f, 0.0f)
                              , glm::vec3 (0.0f, 0.0f,-1.0f) ), sph, &t));
  assert (t > 0.0f);
  assert (intersects (PrimRay ( glm::vec3 (0.0f, 0.0f, 2.0f)
                              , glm::vec3 (0.0f, 0.0f,-1.0f) ), sph, &t));
  assert (t > 0.0f && t < 2.0f);
  assert (intersects (PrimRay ( glm::vec3 (0.0f, 0.0f,-2.0f)
                              , glm::vec3 (0.0f, 0.0f,-1.0f) ), sph, &t) == false);
  assert (intersects (PrimRay ( true
                              , glm::vec3 (0.0f, 0.0f,-2.0f)
                              , glm::vec3 (0.0f, 0.0f,-1.0f) ), sph, &t));
  assert (t < 0.0f && t > -2.0f);

  assert (intersects (PrimRay ( glm::vec3 (0.0f, 1.0f, 0.0f)
                              , glm::vec3 (0.0f,-1.0f, 0.0f) ), pln, &t));
  assert (t > 0.0f);
  assert (intersects (PrimRay ( glm::vec3 (0.0f,-1.0f, 0.0f)
                              , glm::vec3 (0.0f, 1.0f, 0.0f) ), pln, &t));
  assert (t > 0.0f);
  assert (intersects (PrimRay ( glm::vec3 (0.0f, 1.0f, 0.0f)
                              , glm::vec3 (0.0f, 1.0f, 0.0f) ), pln, &t) == false);
  assert (intersects (PrimRay ( true
                              , glm::vec3 (0.0f, 1.0f, 0.0f)
                              , glm::vec3 (0.0f, 1.0f, 0.0f) ), pln, &t));
  assert (t < 0.0f);

  assert (intersects (PrimRay ( glm::vec3 (0.0f, 0.0f, 1.0f)
                              , glm::vec3 (0.0f, 0.0f,-1.0f) ), abx));
  assert (intersects (PrimRay ( glm::vec3 (0.0f, 0.0f,-1.0f)
                              , glm::vec3 (0.0f, 0.0f, 1.0f) ), abx));
  assert (intersects (PrimRay ( glm::vec3 (1.5f, 0.0f, 1.0f)
                              , glm::vec3 (0.0f, 0.0f,-1.0f) ), abx) == false);
  assert (intersects (PrimRay ( glm::vec3 (1.5f, 0.0f,-1.0f)
                              , glm::vec3 (0.0f, 0.0f, 1.0f) ), abx) == false);
  assert (intersects (PrimRay ( glm::vec3 (0.0f, 0.0f,-1.0f)
                              , glm::vec3 (0.0f, 0.0f,-1.0f) ), abx) == false);
  assert (intersects (PrimRay ( glm::vec3 (0.0f, 0.0f, 1.0f)
                              , glm::vec3 (0.0f, 0.0f, 1.0f) ), abx) == false);
  assert (intersects (PrimRay ( true
                              , glm::vec3 (0.0f, 0.0f,-1.0f)
                              , glm::vec3 (0.0f, 0.0f,-1.0f) ), abx));

  assert (intersects (PrimPlane ( glm::vec3 (0.0f, 0.0f, 0.0f)
                                , glm::vec3 (0.0f, 1.0f, 0.0f) ), abx));
  assert (intersects (PrimPlane ( glm::vec3 (0.0f, 0.5f, 0.0f)
                                , glm::vec3 (0.0f, 1.0f, 0.0f) ), abx));
  assert (intersects (PrimPlane ( glm::vec3 (0.0f, 0.6f, 0.0f)
                                , glm::vec3 (0.0f, 1.0f, 0.0f) ), abx) == false);

  assert (intersects (PrimPlane ( glm::vec3 (0.0f, 2.0f, 0.0f)
                                , glm::vec3 (0.0f, 1.0f, 0.0f) ), tri));

  assert (intersects (cyl, glm::vec3 (0.0f, -0.1f, 0.0f)) == false);
  assert (intersects (cyl, glm::vec3 (0.0f,  1.1f, 0.0f)) == false);
  assert (intersects (cyl, glm::vec3 (1.1f,  0.1f, 0.0f)) == false);
  assert (intersects (cyl, glm::vec3 (1.0f,  0.0f, 0.0f)));
  assert (intersects (cyl, glm::vec3 (1.0f,  1.0f, 0.0f)));
  assert (intersects (cyl, glm::vec3 (0.9f,  0.1f, 0.0f)));
  assert (intersects (cyl, glm::vec3 (0.1f,  0.9f, 0.0f)));

  assert (intersects (cne, glm::vec3 (0.0f, -0.1f, 0.0f)) == false);
  assert (intersects (cne, glm::vec3 (1.0f,  0.1f, 0.0f)) == false);
  assert (intersects (cne, glm::vec3 (1.0f,  0.0f, 0.0f)));
  assert (intersects (cne, glm::vec3 (0.5f,  1.0f, 0.0f)));
  assert (intersects (cne, glm::vec3 (0.8f,  0.1f, 0.0f)));
}
