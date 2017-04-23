/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include "distance.hpp"
#include "primitive/cylinder.hpp"
#include "test-distance.hpp"
#include "util.hpp"

void TestDistance::test ()
{
  using Distance::distance;

  const float eps = Util::epsilon ();

  PrimCylinder cyl (glm::vec3 (0.0f, 0.0f, 0.0f), glm::vec3 (1.0f, 0.0f, 0.0f), 0.5f);

  assert (glm::epsilonEqual (distance (cyl, glm::vec3 (0.5f, 1.3f, 0.0f)), 0.8f, eps));

  assert (glm::epsilonEqual (distance (cyl, glm::vec3 (0.1f, 0.0f, 0.0f)), -0.1f, eps));

  assert (glm::epsilonEqual (distance (cyl, glm::vec3 (0.9f, 0.0f, 0.0f)), -0.1f, eps));

  assert (glm::epsilonEqual (distance (cyl, glm::vec3 (0.5f, 0.4f, 0.0f)), -0.1f, eps));

  assert (glm::epsilonEqual (distance (cyl, glm::vec3 (-0.3f, 0.3f, 0.0f)), 0.3f, eps));
  assert (glm::epsilonEqual (distance (cyl, glm::vec3 (1.7f, 0.3f, 0.0f)), 0.7f, eps));
  assert (glm::epsilonEqual (distance (cyl, glm::vec3 (-2.0f, 2.0f, 0.0f)),
                             glm::sqrt ((1.5f * 1.5f) + (2.0f * 2.0f)), eps));
  assert (glm::epsilonEqual (distance (cyl, glm::vec3 (2.0f, 2.0f, 0.0f)),
                             glm::sqrt ((1.5f * 1.5f) + (1.0f * 1.0f)), eps));
}
