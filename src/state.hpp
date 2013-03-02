#ifndef STATE
#define STATE

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "winged-mesh.hpp"
#include "camera.hpp"
#include "cursor-sphere.hpp"

class State {
  public:             State        () {}
                      State        (const State&) = delete;
    State&            operator=    (const State&) = delete;
                      
    static State&     global       ();

    WingedMesh&       mesh         ()             { return this->_mesh;  }
    Camera&           camera       ()             { return this->_camera; }       
    CursorSphere&     cursor       ()             { return this->_cursor; }

    void              initialize   ();
    void              render       ();
    void              setMesh      (const Mesh&);
  private:
    WingedMesh        _mesh;
    Camera            _camera;
    CursorSphere      _cursor;
};
#endif
