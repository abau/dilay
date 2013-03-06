#ifndef STATE
#define STATE

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "winged-mesh.hpp"
#include "camera.hpp"
#include "cursor.hpp"

class State {
  public:             State        () {}
                      State        (const State&) = delete;
    State&            operator=    (const State&) = delete;
                      
    static State&     global       ();

    WingedMesh&       mesh         ()             { return this->_mesh;  }
    Camera&           camera       ()             { return this->_camera; }       
    Cursor&           cursor       ()             { return this->_cursor; }

    void              initialize   ();
    void              render       ();
    void              setMesh      (const Mesh&);
  private:
    WingedMesh        _mesh;
    Camera            _camera;
    Cursor            _cursor;
};
#endif
