/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include "octree.hpp"
#include "primitive/triangle.hpp"
#include "test-octree.hpp"
#include "winged/util.hpp"

void TestOctree::test1 () {
  const unsigned int numSamples = 10000;

  Octree octree;
  octree.setupRoot (glm::vec3 (0.0f), 10.0f);
       
  std::default_random_engine gen; 
  std::uniform_real_distribution <float> unitD   (0.0f , 1.0f);
  std::uniform_real_distribution <float> posD    (-10.0f, 10.0f);
  std::uniform_real_distribution <float> scaleD  (0.00001f, 10.0f);
  std::uniform_real_distribution <float> twoPiD  (0.0f, 2.0f * glm::pi<float> ());

  for (unsigned int i = 0; i < numSamples; i++) {
    glm::vec3 m1 (-1.0f, 0.0f, 0.0f);
    glm::vec3 m2 ( 1.0f, 0.0f, 0.0f);
    glm::vec3 m3 ( 0.0f,-1.0f, 0.0f);

    glm::mat4x4 translationMatrix = glm::translate ( glm::mat4x4 (1.0f)
                                                   , glm::vec3   ( posD (gen)
                                                                 , posD (gen)
                                                                 , posD (gen)
                                                                 ));
    glm::mat4x4 rotationMatrix = glm::rotate ( glm::mat4x4 (1.0f)
                                             , twoPiD (gen)
                                             , glm::normalize (
                                                glm::vec3 ( unitD (gen)
                                                          , unitD (gen)
                                                          , unitD (gen)
                                                          )));

    glm::mat4x4 scalingMatrix = glm::scale ( glm::mat4x4 (1.0f)
                                           , glm::vec3   ( scaleD (gen)
                                                         , scaleD (gen)
                                                         , scaleD (gen)
                                                         ));

    glm::mat4x4 modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;

    glm::vec3 w1 = glm::vec3 (modelMatrix * glm::vec4 (m1, 1.0f));
    glm::vec3 w2 = glm::vec3 (modelMatrix * glm::vec4 (m2, 1.0f));
    glm::vec3 w3 = glm::vec3 (modelMatrix * glm::vec4 (m3, 1.0f));

    octree.addFace (i, PrimTriangle (w1,w2,w3));
  }
  //WingedUtil::printStatistics (octree);

  Octree octree2 (octree);

  OctreeStatistics statistics1 = octree.statistics ();
  OctreeStatistics statistics2 = octree2.statistics ();

  assert (statistics1.numNodes == statistics2.numNodes);
  assert (statistics1.numFaces == statistics2.numFaces);
  assert (statistics1.numDegeneratedFaces == statistics2.numDegeneratedFaces);
  assert (statistics1.minDepth == statistics2.minDepth);
  assert (statistics1.maxDepth == statistics2.maxDepth);
  assert (statistics1.maxFacesPerNode == statistics2.maxFacesPerNode);
  assert (statistics1.numFacesPerDepth == statistics2.numFacesPerDepth);
  assert (statistics1.numNodesPerDepth == statistics2.numNodesPerDepth);

  for (unsigned int i = 0; i < numSamples; i++) {
    octree.deleteFace (i);
  }
  for (unsigned int i = 0; i < numSamples; i++) {
    octree2.deleteFace (i);
  }
}

void TestOctree::test2 () {
/*
  std::cout << "## octree test 2 #######################\n";

  auto f = [] (const glm::vec3& p, float r) -> void {
    ActionFromMesh a;
    a.bufferMesh (false);
    a.writeMesh  (false);

    Mesh mesh = Mesh::icosphere (5);

    mesh.translate (p);
    mesh.scaling   (glm::vec3 (r));

    WingedMesh wMesh;
    a.run (wMesh, mesh);
  };

  std::default_random_engine gen; 
  std::uniform_real_distribution <float> posD    (-10.0f, 10.0f);
  std::uniform_real_distribution <float> radiusD (0.1f, 10.0f);

  for (unsigned int i = 0; i < 100; i++) {
    if (i % 10 == 0) {
      std::cout << "created " << i << " winged meshes\n";
    }
    f (glm::vec3 (posD (gen), posD (gen), posD (gen)), radiusD (gen));
  }

  f (glm::vec3 (8.85855f,-7.48563f,-1.37292f), 1.311f);
*/
}
