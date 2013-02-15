#define  _USE_MATH_DEFINES
#include <cmath>
#include "cursor-sphere.hpp"
#include "opengl.hpp"

void CursorSphere :: initialize () {
  this->_mesh = Mesh :: sphere (this->_radius,10,10);
  this->_mesh.bufferData ();
}

void CursorSphere :: render () {
  this->_mesh.renderWireframe ();
}
