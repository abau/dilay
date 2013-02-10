#include "rendermode.hpp"

#ifndef OPENGL
#define OPENGL

class OpenGL {
  public:
    enum VertexAttributIndex { PositionIndex = 0
                             , NormalIndex   = 1
                             };

    static void initialize       ();
    static void shutdown         ();
    static void setColor         (float,float,float,float);
    static void setColor         (float,float,float);
    static void setMvp           (const GLfloat*);
    static void setProgram       (RenderMode);

    static void safeDeleteArray  (GLuint&);
    static void safeDeleteBuffer (GLuint&);

  private:
    static GLuint _programIds [RenderModeUtil :: numRenderModes];
    static GLuint _mvpId;
    static GLuint _colorId;

    static void   showInfoLog    (GLuint, const char*);
    static GLuint loadShaders    (const char*, const char*);
    static GLuint compileShader  (GLenum, const char*);
    static void   releaseProgram (GLuint);
};

#endif
