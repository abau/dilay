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
    GLuint        programId    () const        { return this->_programId; } 
    Camera&       camera       ()              { return this->_camera; }       
    const Camera& camera       () const        { return this->_camera; }       

    void          setMesh      (WingedMesh* m) { this->_mesh      = m; }
    void          terminate    ()              { this->_isRunning = false; }

    void          initialize   ();
    void          setProgramId (GLuint);

  private:
    bool        _isRunning;
    WingedMesh* _mesh;
    GLuint      _programId;
    Camera      _camera;
};
#endif
