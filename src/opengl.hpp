#include "maybe.hpp"

#ifndef OPENGL
#define OPENGL

class OpenGL {
  public:
    static void initialize         ();
    static void shutdown           ();
    static void loadShaders        (const char*, const char*);
    static void setColor           (float,float,float,float);
    static void setColor           (float,float,float);
    static void setMvp             (const GLfloat*);
    static void setDefaultProgram  ();

    static void   enableCulling    () { glEnable  (GL_CULL_FACE); }
    static void   disableCulling   () { glDisable (GL_CULL_FACE); }
    static void   enableDepthTest  () { glEnable  (GL_DEPTH_TEST); }
    static void   disableDepthTest () { glDisable (GL_DEPTH_TEST); }

  private:
    static GLuint _programId;
    static GLuint _mvpId;
    static GLuint _colorId;

    static void           showInfoLog    (GLuint, const char*);
    static Maybe <GLuint> compileShader  (GLenum, const char*);
    static void           releaseProgram ();
};

#endif
