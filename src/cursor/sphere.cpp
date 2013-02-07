#define  _USE_MATH_DEFINES
#include <cmath>
#include "cursor/sphere.hpp"
#include "opengl.hpp"

int CursorSphere :: sectors = 10;
int CursorSphere :: rings   = 10;

CursorSphere :: CursorSphere (float radius) {

  for (int r = 0; r < CursorSphere :: rings; r++) {
    for (int s = 0; s < CursorSphere :: sectors; s++) {
      float y = sin (-M_PI_2 + M_PI * r * CursorSphere :: rings);
      float x = cos (2*M_PI * s * CursorSphere :: sectors) 
              * sin ( M_PI  * r * CursorSphere :: rings);
      float z = sin (2*M_PI * s * CursorSphere :: sectors) 
              * sin ( M_PI  * r * CursorSphere :: rings );

      this->mesh.addVertex (x * radius, y * radius, z * radius);
    }
  }
  for (int r = 0; r < CursorSphere :: rings - 1; r++) {
    for (int s = 0; s < CursorSphere :: sectors - 1; s++) {
      this->mesh.addIndex ((r * sectors) + s);
      this->mesh.addIndex ((r * sectors) + s + 1);
      this->mesh.addIndex (((r+1) * sectors) + s + 1);
      this->mesh.addIndex (((r+1) * sectors) + s);
    }
  }


      /*
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
  */
  this->mesh.bufferData ();
}

void CursorSphere :: render () {
  /*
  this->mesh.renderBegin ();
  OpenGL :: setColor (0.5f,0.5f,0.5f);
  glDrawElements (GL_QUADS, this->mesh.numIndices (), GL_UNSIGNED_INT, (void*)0);

  this->mesh.renderEnd ();
  */
}
