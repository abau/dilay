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
  //this->_mesh.fromMesh (Mesh :: sphere (1.0f,30,30));
  this->setMesh (Mesh :: cube (1.0f));
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
