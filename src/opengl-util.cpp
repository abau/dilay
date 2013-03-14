#include <glog/logging.h>
#include <glm/glm.hpp>
#include <stdexcept>
#include "opengl-util.hpp"
#include "util.hpp"
#include "macro.hpp"

struct OpenGLUtilImpl {

  void glUniformVec3 (GLuint id, const glm::vec3& v) {
    glUniform3f (id, v.x, v.y, v.z);
  }

  void glUniformVec4 (GLuint id, const glm::vec4& v) {
    glUniform4f (id, v.x, v.y, v.z, v.w);
  }

  void safeDeleteArray (GLuint id) {
    if (glIsVertexArray (id) == GL_TRUE) glDeleteVertexArrays (1,&id);
  }

  void safeDeleteBuffer (GLuint id) {
    if (glIsBuffer (id) == GL_TRUE) glDeleteBuffers (1,&id);
  }

  void safeDeleteShader (GLuint id) {
    if (glIsShader (id) == GL_TRUE) glDeleteShader (id);
  }

  void safeDeleteProgram (GLuint id) {
    if (glIsProgram (id) == GL_TRUE) {
      GLsizei numShaders;
      GLuint  shaderIds[2];

      glGetAttachedShaders (id,2,&numShaders,shaderIds);

      for(GLsizei i = 0; i < numShaders; i++) {
        this->safeDeleteShader (shaderIds[i]);
      }
      glDeleteProgram (id);
    }
  }

  GLuint loadProgram ( const std::string& vertexShader
                     , const std::string& fragmentShader) {

    LOG (INFO) << "Compiling vertex shader '" << vertexShader << "'";
    GLuint vsId = this->compileShader (GL_VERTEX_SHADER,   vertexShader);
    LOG (INFO) << "Compiling fragment shader '" << fragmentShader << "'";
    GLuint fsId = this->compileShader (GL_FRAGMENT_SHADER, fragmentShader);

    GLuint programId = glCreateProgram();
    glAttachShader (programId, vsId);
    glAttachShader (programId, fsId);

    glBindAttribLocation (programId, OpenGLUtil :: PositionIndex, "position");
    glBindAttribLocation (programId, OpenGLUtil :: NormalIndex,   "normal");

    glLinkProgram (programId);

    GLint status;
    glGetProgramiv(programId, GL_LINK_STATUS, &status);

    if (status == GL_FALSE) {
      this->showInfoLog         (programId, "");
      this->safeDeleteProgram   (programId);
      this->safeDeleteShader    (vsId);
      this->safeDeleteShader    (fsId);
      LOG (ERROR) << "Can not link shader program";
      throw (std::runtime_error ("Can not link shader program: see info log"));
    }
    return programId;
  }

  void showInfoLog (GLuint id, const std::string& filePath) {
    const int maxLogLength = 1000;
    char      logBuffer[maxLogLength];
    GLsizei   logLength;
    glGetShaderInfoLog(id, maxLogLength, &logLength, logBuffer);
    if (logLength > 0)
      LOG(INFO) << filePath << ": " << logBuffer;
  }

  GLuint compileShader (GLenum shaderType, const std::string& filePath) {
    GLuint       shaderId   = glCreateShader(shaderType);
    std::string  shaderCode = Util :: readFile (filePath);
    const char* code[1]    = { shaderCode.c_str () };
    glShaderSource(shaderId, 1, code , NULL);
    glCompileShader(shaderId);

    GLint status;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
      this->showInfoLog (shaderId, filePath);
      LOG(ERROR) << filePath << ": can not compile shader";
      throw (std::runtime_error ("Can not compile shader: see info log"));
    }
    return shaderId;
  }
};

GLOBAL(OpenGLUtil)

DELEGATE_CONSTRUCTOR  (OpenGLUtil)
DELEGATE_DESTRUCTOR   (OpenGLUtil)

DELEGATE2 (void,OpenGLUtil,glUniformVec3,GLuint,const glm::vec3&)
DELEGATE2 (void,OpenGLUtil,glUniformVec4,GLuint,const glm::vec4&)
DELEGATE1 (void,OpenGLUtil,safeDeleteArray,GLuint)
DELEGATE1 (void,OpenGLUtil,safeDeleteBuffer,GLuint)
DELEGATE1 (void,OpenGLUtil,safeDeleteShader,GLuint)
DELEGATE1 (void,OpenGLUtil,safeDeleteProgram,GLuint)
DELEGATE2 (GLuint,OpenGLUtil,loadProgram,const std::string&,const std::string&)
