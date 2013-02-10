#include "state.hpp"
#include <fstream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

State& State :: global () {
  static State g;
  return g;
}

void State :: initialize () { 
  this->_camera.initialize ();
}

void State :: render () {
  this->_mesh.render ();
}
