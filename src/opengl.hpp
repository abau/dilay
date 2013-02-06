#include "maybe.hpp"

#ifndef OPENGL
#define OPENGL

class OpenGL {
  public:
    static void initialize         ();
    static void shutdown           ();
    static void toggleWireframe    ();
    static void loadShaders        (const char*, const char*);

    static GLuint programId        () { return OpenGL :: _programId; }
    static GLuint mvpId            () { return OpenGL :: _mvpId; }
    static GLuint vertexId         () { return OpenGL :: _vertexId; }
    static GLuint colorId          () { return OpenGL :: _colorId; }

    static void   enableCulling    () { glEnable  (GL_CULL_FACE); }
    static void   disableCulling   () { glDisable (GL_CULL_FACE); }
    static void   enableDepthTest  () { glEnable  (GL_DEPTH_TEST); }
    static void   disableDepthTest () { glDisable (GL_DEPTH_TEST); }

  private:
    static bool   _isWireframe;
    static GLuint _programId;
    static GLuint _mvpId;
    static GLuint _vertexId;
    static GLuint _colorId;

    static void           showInfoLog    (GLuint, const char*);
    static Maybe <GLuint> compileShader  (GLenum, const char*);
    static void           releaseProgram ();
};

#endif
