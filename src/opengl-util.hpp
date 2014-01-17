#ifndef DILAY_OPENGL_UTIL
#define DILAY_OPENGL_UTIL

#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
#include <glm/fwd.hpp>

namespace OpenGLUtil {
  enum VertexAttributIndex { PositionIndex = 0
                           , NormalIndex   = 1
                           };

  void   glUniformVec3     (GLuint, const glm::vec3&);
  void   glUniformVec4     (GLuint, const glm::vec4&);
  void   safeDeleteArray   (GLuint&);
  void   safeDeleteBuffer  (GLuint&);
  void   safeDeleteShader  (GLuint&);
  void   safeDeleteProgram (GLuint&);
  GLuint loadProgram       (const std::string&, const std::string&);
};

#endif
