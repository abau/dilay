#include <iostream>
#include <random>
#include <chrono>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <QCoreApplication>
#include "octree.hpp"
#include "primitive/triangle.hpp"
#include "winged/face.hpp"
#include "winged/util.hpp"
#include "winged/mesh.hpp"
#include "util.hpp"
#include "mesh.hpp"

void octreeTest1 ();
void octreeTest2 ();
void interimTest ();

int main () {
  QCoreApplication::setApplicationName ("dilay");

  octreeTest1 ();
  //octreeTest2 ();

  std::cout << "all tests run successfully\n";
  return 0;
}

void octreeTest1 () {
  std::cout << "## octree test 1 #######################\n";
  Octree octree;
  octree.setupRoot (glm::vec3 (0.0f), 10.0f);
       
  std::default_random_engine gen; 
  std::uniform_real_distribution <float> unitD   (0.0f , 1.0f);
  std::uniform_real_distribution <float> posD    (-10.0f, 10.0f);
  std::uniform_real_distribution <float> scaleD  (0.00001f, 10.0f);
  std::uniform_real_distribution <float> twoPiD  (0.0f, 2.0f * glm::pi<float> ());

  for (unsigned int i = 0; i < 10000; i++) {
    if (i % 1000 == 0) {
      std::cout << "inserted " << i << " faces in octree\n";
    }
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

    octree.insertFace (WingedFace (), PrimTriangle (w1,w2,w3));
  }
  WingedUtil::printStatistics (octree);
}

/*
void octreeTest2 () {
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
}
*/
