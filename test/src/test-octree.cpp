/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include "dynamic/octree.hpp"
#include "primitive/triangle.hpp"
#include "test-octree.hpp"

void TestOctree::test ()
{
  const unsigned int numSamples = 10000;

  DynamicOctree octree;
  octree.setupRoot (glm::vec3 (0.0f), 10.0f);

  std::default_random_engine            gen;
  std::uniform_real_distribution<float> unitD (0.0f, 1.0f);
  std::uniform_real_distribution<float> posD (-10.0f, 10.0f);
  std::uniform_real_distribution<float> scaleD (0.00001f, 10.0f);
  std::uniform_real_distribution<float> twoPiD (0.0f, 2.0f * glm::pi<float> ());

  for (unsigned int i = 0; i < numSamples; i++)
  {
    const glm::vec3 m1 (-1.0f, 0.0f, 0.0f);
    const glm::vec3 m2 (1.0f, 0.0f, 0.0f);
    const glm::vec3 m3 (0.0f, -1.0f, 0.0f);

    const glm::mat4x4 translationMatrix =
      glm::translate (glm::mat4x4 (1.0f), glm::vec3 (posD (gen), posD (gen), posD (gen)));
    const glm::mat4x4 rotationMatrix =
      glm::rotate (glm::mat4x4 (1.0f), twoPiD (gen),
                   glm::normalize (glm::vec3 (unitD (gen), unitD (gen), unitD (gen))));

    const glm::mat4x4 scalingMatrix =
      glm::scale (glm::mat4x4 (1.0f), glm::vec3 (scaleD (gen), scaleD (gen), scaleD (gen)));

    const glm::mat4x4 modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;

    const glm::vec3    w1 = glm::vec3 (modelMatrix * glm::vec4 (m1, 1.0f));
    const glm::vec3    w2 = glm::vec3 (modelMatrix * glm::vec4 (m2, 1.0f));
    const glm::vec3    w3 = glm::vec3 (modelMatrix * glm::vec4 (m3, 1.0f));
    const PrimTriangle tri = PrimTriangle (w1, w2, w3);

    octree.addElement (i, tri.center (), tri.maxDimExtent ());
  }
  for (unsigned int i = 0; i < numSamples; i++)
  {
    octree.deleteElement (i);
  }
}
