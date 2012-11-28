#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "winged-mesh.hpp"
#include "camera.hpp"

#ifndef STATE
#define STATE

class State {
  public:
    State ();
    ~State ();

    static State& global ();

    bool          isRunning    () const        { return this->_isRunning; }
    WingedMesh*   mesh         () const        { return this->_mesh; }
    Camera&       camera       ()              { return this->_camera; }       
    const Camera& camera       () const        { return this->_camera; }       

    void          setMesh      (WingedMesh* m) { this->_mesh      = m; }
    void          terminate    ()              { this->_isRunning = false; }

    void          initialize   ();
  private:
    bool        _isRunning;
    WingedMesh* _mesh;
    Camera      _camera;
};
#endif
