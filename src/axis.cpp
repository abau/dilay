#include "axis.hpp"
#include "opengl.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "state.hpp"

Axis :: Axis () : length (0.5f) { }

void Axis :: initialize () {
  this->mesh.addVertex (glm::vec3 (0.0f        , 0.0f        , 0.0f        ));
  this->mesh.addVertex (glm::vec3 (this->length, 0.0f        , 0.0f        ));
  this->mesh.addVertex (glm::vec3 (0.0f        , this->length, 0.0f        ));
  this->mesh.addVertex (glm::vec3 (0.0f        , 0.0f        , this->length));

  this->mesh.addIndex (0);
  this->mesh.addIndex (1);
  this->mesh.addIndex (0);
  this->mesh.addIndex (2);
  this->mesh.addIndex (0);
  this->mesh.addIndex (3);

  this->mesh.bufferData ();
}

void Axis :: render () {
  this->mesh.renderBegin ();

  State :: global ().camera ().updateProjection (0,0,200,200);
  State :: global ().camera ().rotationProjection ();

  glDisable (GL_DEPTH_TEST);

  OpenGL :: setColor (1.0f,0.0f,0.0f);
  glDrawElements (GL_LINES, 2, GL_UNSIGNED_INT, (void*)(GLvoid*)(sizeof(GLuint) * 0));

  OpenGL :: setColor (0.0f,1.0f,0.0f);
  glDrawElements (GL_LINES, 2, GL_UNSIGNED_INT, (void*)(GLvoid*)(sizeof(GLuint) * 2));

  OpenGL :: setColor (0.0f,0.0f,1.0f);
  glDrawElements (GL_LINES, 2, GL_UNSIGNED_INT, (void*)(GLvoid*)(sizeof(GLuint) * 4));

  glEnable (GL_DEPTH_TEST);
  this->mesh.renderEnd ();
}
