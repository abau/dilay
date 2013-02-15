#include "state.hpp"
#include <fstream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

State& State :: global () {
  static State g;
  return g;
}

void State :: initialize () { 
  this->_mesh.fromMesh (Mesh :: sphere (1.0f,30,30));
  //State :: global ().setMesh (Mesh :: cube (1.0f));
  this->_mesh.bufferData ();
  this->_camera.initialize ();
  this->_cursor.initialize ();
}

void State :: render () {
  this->_mesh.render ();
  this->_cursor.render ();
}
