#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "winged-mesh.hpp"
#include "camera.hpp"

#ifndef STATE
#define STATE

class State {
  public:
    static State&     global       ();

    WingedMesh&       mesh         ()              { return this->_mesh; }
    const WingedMesh& mesh         () const        { return this->_mesh; }
    Camera&           camera       ()              { return this->_camera; }       
    const Camera&     camera       () const        { return this->_camera; }       

    void              initialize   ();
    void              setMesh      (const Mesh& m) { this->_mesh.fromMesh (m); }
  private:
    WingedMesh  _mesh;
    Camera      _camera;
};
#endif
