#include <fstream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "state.hpp"
#include "winged-mesh-util.hpp"

State& State :: global () {
  static State g;
  return g;
}

void State :: initialize () { 
  this->_config.initialize ("dilay.config");
  this->setMesh (Mesh :: sphere (1.0f,10,20));
  this->_mesh.bufferData ();
  this->_camera.initialize ();
  this->_cursor.initialize ();
}

void State :: render () {
  this->_mesh.render ();
  this->_cursor.render ();
}

void State :: setMesh (const Mesh& mesh) {
  WingedMeshUtil :: fromMesh (this->_mesh,mesh);
}
