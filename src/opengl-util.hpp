#ifndef DILAY_OPENGL_UTIL
#define DILAY_OPENGL_UTIL

#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
#include "fwd-glm.hpp"

class OpenGLUtilImpl;

class OpenGLUtil {
  public:
    static OpenGLUtil& global ();

    enum VertexAttributIndex { PositionIndex = 0
                             , NormalIndex   = 1
                             };

    static void   glUniformVec3     (GLuint, const glm::vec3&);
    static void   glUniformVec4     (GLuint, const glm::vec4&);
    static void   safeDeleteArray   (GLuint&);
    static void   safeDeleteBuffer  (GLuint&);
    static void   safeDeleteShader  (GLuint&);
    static void   safeDeleteProgram (GLuint&);
    static GLuint loadProgram       (const std::string&, const std::string&);

  private:
          OpenGLUtil            ();
          OpenGLUtil            (const OpenGLUtil&) = delete;
    const OpenGLUtil& operator= (const OpenGLUtil&) = delete;
         ~OpenGLUtil            ();

    OpenGLUtilImpl* impl;
};

#endif
