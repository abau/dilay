#include "state.hpp"
#include <fstream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

State :: State () : _isRunning (false), _mesh (0), _programId (0) {}

State :: ~State () {
  if (this->_mesh != 0) 
    delete (this->_mesh);
  if (glIsProgram (this->_programId) == GL_TRUE)
    glDeleteProgram (this->_programId);
}

State& State :: global () {
  static State g;
  return g;
}

void State :: initialize () { 
  this->_isRunning  = true;
  this->_camera.initialize ();
}

void State :: setProgramId (GLuint id) {
  this->_programId = id;
  this->_camera.setMvpId (glGetUniformLocation(id, "mvp"));
}
