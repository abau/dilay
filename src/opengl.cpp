#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>
#include <stdexcept>
#include "opengl.hpp"

GLuint  OpenGL :: _mvpId       = -1;
GLuint  OpenGL :: _programId   = -1;
GLuint  OpenGL :: _colorId     = -1;

void OpenGL :: initialize () {
  static bool isInitialized = false;

  if ( ! isInitialized ) {
    std::cout << "Initializing OpenGL\n";
    GLenum err = glewInit ();
    if (err  != GLEW_OK ) {
      std::string e1 = std::string ("Error while initializing glew: ");
      std::string e2 = std::string ((const char*)glewGetErrorString (err));
      throw (std::runtime_error (e1 + e2));
    }

    glClearColor(0.1f, 0.1f, 0.3f, 0.0f);
    glDepthFunc (GL_LEQUAL); 

    OpenGL :: enableCulling   ();
    OpenGL :: enableDepthTest ();
    OpenGL :: loadShaders     ("shader/vertex.shader", "shader/fragment.shader" );

    isInitialized = true;
  }
}

void OpenGL :: shutdown () {
  OpenGL :: releaseProgram ();
}

void OpenGL :: loadShaders ( const char* vertexShader, const char* fragmentShader) {
  OpenGL :: releaseProgram ();

  Maybe <GLuint> vsId = OpenGL :: compileShader (GL_VERTEX_SHADER, vertexShader);
  Maybe <GLuint> fsId = OpenGL :: compileShader (GL_FRAGMENT_SHADER, fragmentShader);

  if (vsId.isDefined () && fsId.isDefined ()) {

    GLuint programId = glCreateProgram();
    glAttachShader (programId, vsId.data ());
    glAttachShader (programId, fsId.data ());
    glLinkProgram  (programId);

    GLint status;
    glGetProgramiv(programId, GL_LINK_STATUS, &status);

    glDeleteShader(vsId.data ());
    glDeleteShader(fsId.data ());

    if (status == GL_TRUE) {
      OpenGL :: _programId = programId;
      OpenGL :: _mvpId     = glGetUniformLocation (programId, "mvp");
      OpenGL :: _colorId   = glGetUniformLocation (programId, "color");
      return;
    }
    OpenGL :: showInfoLog (programId, "");
  }
  else if (vsId.isDefined ()) glDeleteShader(vsId.data ());
  else if (fsId.isDefined ()) glDeleteShader(fsId.data ());

  throw (std::runtime_error ("Can not load shaders: see info log above"));
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


void OpenGL :: setDefaultProgram  () {
  glUseProgram(OpenGL :: _programId);
}

void OpenGL :: showInfoLog (GLuint id, const char* filePath) {
  GLsizei infoLogLength;
  glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);

  if (infoLogLength > 0) {
    GLchar* infoLog = new char[infoLogLength];
    glGetShaderInfoLog(id, infoLogLength, NULL, infoLog);
    std::cout << filePath << std::endl << infoLog << std::endl;
    delete[] (infoLog);
  }
}

Maybe <GLuint> OpenGL :: compileShader (GLenum shaderType, const char* filePath) {
  GLuint       shaderId   = glCreateShader(shaderType);
  std::string  shaderCode = Util :: readFile (filePath);
  const char* code[1]    = { shaderCode.c_str () };
  glShaderSource(shaderId, 1, code , NULL);
  glCompileShader(shaderId);

  GLint status;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    OpenGL :: showInfoLog (shaderId, filePath);
    return Maybe <GLuint> :: nothing ();
  }
  return Maybe <GLuint> (shaderId);
}

void OpenGL :: releaseProgram () {
  if (glIsProgram (_programId) == GL_TRUE) {
    glDeleteProgram (_programId);
    OpenGL :: _programId = -1;
  }
}
