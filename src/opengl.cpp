#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <stdexcept>
#include "opengl.hpp"
#include "util.hpp"

GLuint  OpenGL :: _programIds [RenderModeUtil :: numRenderModes];
GLuint  OpenGL :: _mvpId;
GLuint  OpenGL :: _colorId;
GLuint  OpenGL :: _light1PositionId;
GLuint  OpenGL :: _light1ColorId;
GLuint  OpenGL :: _light1IrradianceId;

void OpenGL :: initialize () {
  static bool isInitialized = false;

  if ( ! isInitialized ) {
    std::cout << "Initializing OpenGL... ";
    GLenum err = glewInit ();
    if (err  != GLEW_OK ) {
      std::string e1 = std::string ("Error while initializing glew: ");
      std::string e2 = std::string ((const char*)glewGetErrorString (err));
      throw (std::runtime_error (e1 + e2));
    }

    glClearColor(0.1f, 0.1f, 0.3f, 0.0f);
    glDepthFunc (GL_LEQUAL); 

    glEnable  (GL_CULL_FACE);
    glEnable  (GL_DEPTH_TEST); 

    OpenGL :: _programIds [RenderSolid] = 
      OpenGL :: loadShaders ( "shader/light-vertex.shader"
                            , "shader/light-fragment.shader" 
                            );
    OpenGL :: _programIds [RenderWireframe] = 
      OpenGL :: loadShaders ( "shader/simple-vertex.shader"
                            , "shader/simple-fragment.shader" 
                            );
    OpenGL :: _programIds [RenderFlat] = 
      OpenGL :: loadShaders ( "shader/flat-light-vertex.shader"
                            , "shader/flat-light-fragment.shader" 
                            );
    isInitialized = true;
    std::cout << "done" << std::endl;
  }
}

void OpenGL :: shutdown () {
  for (int rm = 0; rm < RenderModeUtil :: numRenderModes; rm++)
    OpenGL :: releaseProgram (rm);
}

void OpenGL :: setColor (float r, float g, float b, float a) {
  glUniform4f (OpenGL :: _colorId, r, g, b, a);
}

void OpenGL :: setColor (float r, float g, float b) {
  OpenGL :: setColor (r, g, b, 1.0f);
}

void OpenGL :: setMvp (const GLfloat* mvp) {
  glUniformMatrix4fv (OpenGL :: _mvpId, 1, GL_FALSE, mvp);
}

void OpenGL :: setProgram (RenderMode renderMode) {
  GLuint programId            = OpenGL :: _programIds [renderMode];
  glUseProgram (programId);
  OpenGL :: _mvpId            = glGetUniformLocation (programId, "mvp");
  OpenGL :: _colorId          = glGetUniformLocation (programId, "color");
  OpenGL :: _light1PositionId = glGetUniformLocation (programId, "light1Position");
  OpenGL :: _light1ColorId    = glGetUniformLocation (programId, "light1Color");
  OpenGL :: _light1IrradianceId = glGetUniformLocation (programId, "light1Irradiance");
}

void OpenGL :: setViewLight1 (const ViewLight& light) {
  OpenGL :: glUniformVec3 (OpenGL :: _light1PositionId,   light.position ());
  OpenGL :: glUniformVec3 (OpenGL :: _light1ColorId,      light.color ());
            glUniform1f   (OpenGL :: _light1IrradianceId, light.irradiance ());
}

void OpenGL :: safeDeleteArray (GLuint& id) {
  if (glIsVertexArray (id) == GL_TRUE) {
    glDeleteVertexArrays (1,&id);
    id = 0;
  }
}

void OpenGL :: safeDeleteBuffer (GLuint& id) {
  if (glIsBuffer (id) == GL_TRUE) {
    glDeleteBuffers (1,&id);
    id = 0;
  }
}

void OpenGL :: showInfoLog (GLuint id, const char* filePath) {
  const int maxLogLength = 1000;
  char      logBuffer[maxLogLength];
  GLsizei   logLength;
  glGetShaderInfoLog(id, maxLogLength, &logLength, logBuffer);
  if (logLength > 0)
    std::cout << filePath << ": " << logBuffer << std::endl;
}

GLuint OpenGL :: loadShaders (const char* vertexShader, const char* fragmentShader) {
  GLuint vsId = OpenGL :: compileShader (GL_VERTEX_SHADER, vertexShader);
  GLuint fsId = OpenGL :: compileShader (GL_FRAGMENT_SHADER, fragmentShader);

  GLuint programId = glCreateProgram();
  glAttachShader (programId, vsId);
  glAttachShader (programId, fsId);

  glBindAttribLocation (programId, OpenGL :: PositionIndex, "position");
  glBindAttribLocation (programId, OpenGL :: NormalIndex,   "normal");

  glLinkProgram (programId);

  GLint status;
  glGetProgramiv(programId, GL_LINK_STATUS, &status);

  glDeleteShader(vsId);
  glDeleteShader(fsId);

  if (status == GL_FALSE) {
    OpenGL :: showInfoLog (programId, "");
    glDeleteShader(vsId);
    glDeleteShader(fsId);
    throw (std::runtime_error ("Can not link shader shader program: see info log above"));
  }
  return programId;
}

GLuint OpenGL :: compileShader (GLenum shaderType, const char* filePath) {
  GLuint       shaderId   = glCreateShader(shaderType);
  std::string  shaderCode = Util :: readFile (filePath);
  const char* code[1]    = { shaderCode.c_str () };
  glShaderSource(shaderId, 1, code , NULL);
  glCompileShader(shaderId);

  GLint status;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    OpenGL :: showInfoLog (shaderId, filePath);
    throw (std::runtime_error ("Can not compile shader: see info log above"));
  }
  return shaderId;
}

void OpenGL :: releaseProgram (GLuint id) {
  if (glIsProgram (id) == GL_TRUE) {
    glDeleteProgram (id);
  }
}

void OpenGL :: glUniformVec3 (GLuint id, const glm::vec3& v) {
  glUniform3f (id, v.x, v.y, v.z);
}
