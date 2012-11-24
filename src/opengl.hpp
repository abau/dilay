#include "maybe.hpp"

#ifndef OPENGL
#define OPENGL

class OpenGL {
  private:
    static bool isWireframe;

    static void           showInfoLog   (GLuint, const char*);
    static Maybe <GLuint> compileShader (GLenum, const char*);

  public:
    static void initialize      (int,int);
    static void shutdown        ();
    static void toggleWireframe ();
    static void loadShaders     (const char*, const char*);
};

#endif
