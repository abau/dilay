#include <GL/glew.h>
#include <GL/glfw.h>
#include <GL/gl.h>
#include <iostream>
#include <stdexcept>

#include "opengl.hpp"
#include "state.hpp"

bool OpenGL :: isWireframe = false;

void onResize (int width, int height) {
  State :: global().camera ().updateResolution (width,height);
}

void OpenGL :: initialize (int width, int height) {
  std::cout << "Initializing OpenGL\n";
  if ( !glfwInit() ) {
    std::cout << "Error while initializing glfw\n";
    exit (EXIT_FAILURE);
  }
  if ( !glfwOpenWindow(width, height, 0,0,0,0, 32,0, GLFW_WINDOW ) ) {
    std::cout << "Error while opening window using glfw\n";
    glfwTerminate ();
    exit (EXIT_FAILURE);
  }

  if ( glewInit () != GLEW_OK ) {
    std::cout << "Error while initializing glew\n";
    exit (EXIT_FAILURE);
  }

  glEnable      (GL_DEPTH_TEST);
  glDepthFunc   (GL_LESS);
  glEnable      (GL_CULL_FACE);

  onResize (width,height);
  glfwSetWindowSizeCallback (onResize);
}

void OpenGL :: shutdown () {
  std::cout << "Shuting down OpenGL\n";
  glfwTerminate();
}

void OpenGL :: toggleWireframe () {
  isWireframe = ! isWireframe;
  if (isWireframe)
    glPolygonMode (GL_FRONT, GL_LINE);
  else
    glPolygonMode (GL_FRONT, GL_FILL);
}

void OpenGL :: loadShaders ( const char* vertexShader, const char* fragmentShader) {
  Maybe <GLuint> vsId = OpenGL :: compileShader (GL_VERTEX_SHADER, vertexShader);
  Maybe <GLuint> fsId = OpenGL :: compileShader (GL_FRAGMENT_SHADER, fragmentShader);

  if (vsId.isDefined () && fsId.isDefined ()) {

    GLuint programId = glCreateProgram();
    glAttachShader (programId, vsId.data ());
    glAttachShader (programId, fsId.data ());
    glLinkProgram  (programId);

    OpenGL :: showInfoLog (programId, "");

    GLint status;
    glGetProgramiv(programId, GL_LINK_STATUS, &status);

    glDeleteShader(vsId.data ());
    glDeleteShader(fsId.data ());

    if (status == GL_TRUE) {
      State :: global ().setProgramId (programId);
      return;
    }
  }
  else if (vsId.isDefined ()) glDeleteShader(vsId.data ());
  else if (fsId.isDefined ()) glDeleteShader(fsId.data ());

  throw (std::runtime_error ("Can not load shaders: see info log above"));
}

void OpenGL :: showInfoLog (GLuint id, const char* filePath) {
  GLsizei infoLogLength;
  glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);

  if (infoLogLength > 1) {
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

  OpenGL :: showInfoLog (shaderId, filePath);

  GLint status;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE)
    return Maybe <GLuint> :: nothing ();
  return Maybe <GLuint> (shaderId);
}
