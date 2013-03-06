#define  _USE_MATH_DEFINES
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include "cursor.hpp"
#include "opengl.hpp"

void Cursor :: initialize () {
  assert (this->_sectors > 2);
  float sectorStep = 2.0f * M_PI / float (this->_sectors);
  float theta      = 0.0f;

  this->_mesh.reset ();

  for (unsigned int s = 0; s < this->_sectors; s++) {
    float x = this->_radius * sin (theta);
    float z = this->_radius * cos (theta);

    this->_mesh.addVertex (glm::vec3 (x,0.0f,z));
    if (s > 0) {
      this->_mesh.addIndex  (s-1);
      this->_mesh.addIndex  (s);
    }
    theta += sectorStep;
  }
  this->_mesh.addIndex   (0);
  this->_mesh.addIndex   (this->_sectors-1);
  this->_mesh.bufferData ();
}

void Cursor :: setNormal (const glm::vec3& v) {
  glm::vec3 axis  = glm::cross (glm::vec3 (0.0f,1.0f,0.0f),v);
  float     angle = glm::degrees ( glm::acos ( 
                                   glm::dot  (glm::vec3 (0.0f,1.0f,0.0f),v)));

  this->_mesh.setRotation (glm::rotate (glm::mat4(1.0f), angle, axis));
}

void Cursor :: render () {
  if (this->_isEnabled) {
    this->_mesh.renderBegin ();

    glDisable (GL_DEPTH_TEST); 

    OpenGL :: setColor (1.0f, 0.0f, 0.0f);
    glDrawElements     (GL_LINES, this->_mesh.numIndices (), GL_UNSIGNED_INT, (void*)0);

    glEnable (GL_DEPTH_TEST); 

    this->_mesh.renderEnd ();
  }
}
