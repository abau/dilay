#define  _USE_MATH_DEFINES
#include <cmath>
#include "cursor/sphere.hpp"
#include "opengl.hpp"

int CursorSphere :: steps = 24;

CursorSphere :: CursorSphere () {
  const float angleStep = 2.0f * M_PI / float (CursorSphere :: steps);
  const float radius    = 1.0f;
        float angle     = 0.0f;

  this->mesh.addVertex ( 0.0f, 0.0f, 0.0f );
  this->mesh.addIndex (0);

  for (int i = 0; i < CursorSphere :: steps; i++) {
    angle += angleStep;
    this->mesh.addVertex (radius * cosf (angle), 0.0f, radius * sinf (angle));
    this->mesh.addIndex (i);
    //this->mesh.addIndex ((i + 1) % CursorSphere :: steps);
  }
  this->mesh.bufferData ();
}

void CursorSphere :: render () {
  /*
  this->mesh.renderBegin ();
  glUniform4f (OpenGL :: colorId (), 0.5f,0.5f,0.5f,0.5f);
  glDrawElements (GL_TRIANGLE_FAN, this->mesh.numIndices (), GL_UNSIGNED_INT, (void*)0);

  this->mesh.renderEnd ();
  */
}
