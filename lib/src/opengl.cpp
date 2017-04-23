/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QOpenGLContext>
#include <QOpenGLExtensions>
#include <QOpenGLFunctions_2_1>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include "opengl.hpp"
#include "shader.hpp"
#include "util.hpp"

#define DELEGATE_GL_CONSTANT(method, constant) \
  unsigned int method ()                       \
  {                                            \
    return constant;                           \
  }
#define DELEGATE_GL(r, method) \
  r method ()                  \
  {                            \
    return fun->method ();     \
  }
#define DELEGATE1_GL(r, method, t1) \
  r method (t1 a1)                  \
  {                                 \
    return fun->method (a1);        \
  }
#define DELEGATE2_GL(r, method, t1, t2) \
  r method (t1 a1, t2 a2)               \
  {                                     \
    return fun->method (a1, a2);        \
  }
#define DELEGATE3_GL(r, method, t1, t2, t3) \
  r method (t1 a1, t2 a2, t3 a3)            \
  {                                         \
    return fun->method (a1, a2, a3);        \
  }
#define DELEGATE4_GL(r, method, t1, t2, t3, t4) \
  r method (t1 a1, t2 a2, t3 a3, t4 a4)         \
  {                                             \
    return fun->method (a1, a2, a3, a4);        \
  }
#define DELEGATE5_GL(r, method, t1, t2, t3, t4, t5) \
  r method (t1 a1, t2 a2, t3 a3, t4 a4, t5 a5)      \
  {                                                 \
    return fun->method (a1, a2, a3, a4, a5);        \
  }
#define DELEGATE6_GL(r, method, t1, t2, t3, t4, t5, t6) \
  r method (t1 a1, t2 a2, t3 a3, t4 a4, t5 a5, t6 a6)   \
  {                                                     \
    return fun->method (a1, a2, a3, a4, a5, a6);        \
  }

namespace OpenGL
{
  static_assert (sizeof (unsigned int) >= 4, "type does not meet size required by OpenGL");
  static_assert (sizeof (int) >= 4, "type does not meet size required by OpenGL");
  static_assert (sizeof (float) >= 4, "type does not meet size required by OpenGL");

  static QOpenGLFunctions_2_1*                                  fun = nullptr;
  static std::unique_ptr<QOpenGLExtension_EXT_geometry_shader4> gsFun;

  void setDefaultFormat ()
  {
    QSurfaceFormat format;

    format.setVersion (2, 1);
    format.setDepthBufferSize (24);
    format.setStencilBufferSize (1);
    format.setProfile (QSurfaceFormat::NoProfile);
    format.setRenderableType (QSurfaceFormat::OpenGL);

    QSurfaceFormat::setDefaultFormat (format);
  }

  void initializeFunctions ()
  {
    fun = QOpenGLContext::currentContext ()->versionFunctions<QOpenGLFunctions_2_1> ();
    if (fun == nullptr)
    {
      DILAY_PANIC ("could not obtain OpenGL 2.1 context")
    }
    fun->initializeOpenGLFunctions ();

    if (OpenGL::supportsGeometryShader ())
    {
      gsFun = std::make_unique<QOpenGLExtension_EXT_geometry_shader4> ();
      if (gsFun == nullptr)
      {
        DILAY_PANIC ("could not initialize GL_EXT_geometry_shader4 extension")
      }
      gsFun->initializeOpenGLFunctions ();
    }
  }

  DELEGATE_GL_CONSTANT (Always, GL_ALWAYS);
  DELEGATE_GL_CONSTANT (ArrayBuffer, GL_ARRAY_BUFFER);
  DELEGATE_GL_CONSTANT (Back, GL_BACK);
  DELEGATE_GL_CONSTANT (Blend, GL_BLEND);
  DELEGATE_GL_CONSTANT (BufferSize, GL_BUFFER_SIZE);
  DELEGATE_GL_CONSTANT (ColorBufferBit, GL_COLOR_BUFFER_BIT);
  DELEGATE_GL_CONSTANT (CullFace, GL_CULL_FACE);
  DELEGATE_GL_CONSTANT (CW, GL_CW);
  DELEGATE_GL_CONSTANT (CCW, GL_CCW);
  DELEGATE_GL_CONSTANT (Decr, GL_DECR);
  DELEGATE_GL_CONSTANT (DecrWrap, GL_DECR_WRAP);
  DELEGATE_GL_CONSTANT (DepthBufferBit, GL_DEPTH_BUFFER_BIT);
  DELEGATE_GL_CONSTANT (DepthTest, GL_DEPTH_TEST);
  DELEGATE_GL_CONSTANT (DstColor, GL_DST_COLOR);
  DELEGATE_GL_CONSTANT (ElementArrayBuffer, GL_ELEMENT_ARRAY_BUFFER);
  DELEGATE_GL_CONSTANT (Equal, GL_EQUAL);
  DELEGATE_GL_CONSTANT (Fill, GL_FILL);
  DELEGATE_GL_CONSTANT (Float, GL_FLOAT);
  DELEGATE_GL_CONSTANT (Front, GL_FRONT);
  DELEGATE_GL_CONSTANT (FrontAndBack, GL_FRONT_AND_BACK);
  DELEGATE_GL_CONSTANT (FuncAdd, GL_FUNC_ADD);
  DELEGATE_GL_CONSTANT (Greater, GL_GREATER);
  DELEGATE_GL_CONSTANT (Incr, GL_INCR);
  DELEGATE_GL_CONSTANT (IncrWrap, GL_INCR_WRAP);
  DELEGATE_GL_CONSTANT (Invert, GL_INVERT);
  DELEGATE_GL_CONSTANT (Keep, GL_KEEP);
  DELEGATE_GL_CONSTANT (LEqual, GL_LEQUAL);
  DELEGATE_GL_CONSTANT (Line, GL_LINE);
  DELEGATE_GL_CONSTANT (Lines, GL_LINES);
  DELEGATE_GL_CONSTANT (Never, GL_NEVER);
  DELEGATE_GL_CONSTANT (PolygonOffsetFill, GL_POLYGON_OFFSET_FILL);
  DELEGATE_GL_CONSTANT (Replace, GL_REPLACE);
  DELEGATE_GL_CONSTANT (StaticDraw, GL_STATIC_DRAW);
  DELEGATE_GL_CONSTANT (StencilBufferBit, GL_STENCIL_BUFFER_BIT);
  DELEGATE_GL_CONSTANT (StencilTest, GL_STENCIL_TEST);
  DELEGATE_GL_CONSTANT (Triangles, GL_TRIANGLES);
  DELEGATE_GL_CONSTANT (UnsignedInt, GL_UNSIGNED_INT);
  DELEGATE_GL_CONSTANT (Zero, GL_ZERO);

  DELEGATE2_GL (void, glBindBuffer, unsigned int, unsigned int)
  DELEGATE1_GL (void, glBlendEquation, unsigned int)
  DELEGATE2_GL (void, glBlendFunc, unsigned int, unsigned int)
  DELEGATE4_GL (void, glBufferData, unsigned int, unsigned int, const void*, unsigned int)
  DELEGATE4_GL (void, glBufferSubData, unsigned int, unsigned int, unsigned int, const void*)
  DELEGATE1_GL (void, glClear, unsigned int)
  DELEGATE4_GL (void, glClearColor, float, float, float, float)
  DELEGATE1_GL (void, glClearStencil, int)
  DELEGATE4_GL (void, glColorMask, bool, bool, bool, bool)
  DELEGATE1_GL (void, glCullFace, unsigned int)
  DELEGATE1_GL (void, glDepthFunc, unsigned int)
  DELEGATE1_GL (void, glDepthMask, bool)
  DELEGATE1_GL (void, glDisable, unsigned int)
  DELEGATE1_GL (void, glDisableVertexAttribArray, unsigned int)
  DELEGATE4_GL (void, glDrawElements, unsigned int, unsigned int, unsigned int, const void*)
  DELEGATE1_GL (void, glEnable, unsigned int)
  DELEGATE1_GL (void, glEnableVertexAttribArray, unsigned int)
  DELEGATE1_GL (void, glFrontFace, unsigned int)
  DELEGATE2_GL (void, glGenBuffers, unsigned int, unsigned int*)
  DELEGATE3_GL (void, glGetBufferParameteriv, unsigned int, unsigned int, int*)
  DELEGATE2_GL (int, glGetUniformLocation, unsigned int, const char*)
  DELEGATE1_GL (bool, glIsBuffer, unsigned int)
  DELEGATE1_GL (bool, glIsProgram, unsigned int)
  DELEGATE2_GL (void, glPolygonMode, unsigned int, unsigned int)
  DELEGATE2_GL (void, glPolygonOffset, float, float)
  DELEGATE3_GL (void, glStencilFunc, unsigned int, int, unsigned int)
  DELEGATE3_GL (void, glStencilOp, unsigned int, unsigned int, unsigned int)
  DELEGATE2_GL (void, glUniform1f, int, float)
  DELEGATE4_GL (void, glUniformMatrix3fv, int, unsigned int, bool, const float*)
  DELEGATE4_GL (void, glUniformMatrix4fv, int, unsigned int, bool, const float*)
  DELEGATE1_GL (void, glUseProgram, unsigned int)
  DELEGATE6_GL (void, glVertexAttribPointer, unsigned int, int, unsigned int, bool, unsigned int,
                const void*)
  DELEGATE4_GL (void, glViewport, unsigned int, unsigned int, unsigned int, unsigned int)

  bool supportsGeometryShader ()
  {
    return QOpenGLContext::currentContext ()->hasExtension (QByteArray ("GL_EXT_geometry_shader4"));
  }

  void glUniformVec3 (unsigned int id, const glm::vec3& v)
  {
    fun->glUniform3f (id, v.x, v.y, v.z);
  }
  void glUniformVec4 (unsigned int id, const glm::vec4& v)
  {
    fun->glUniform4f (id, v.x, v.y, v.z, v.w);
  }

  void safeDeleteBuffer (unsigned int& id)
  {
    if (id > 0)
    {
      fun->glDeleteBuffers (1, &id);
    }
    id = 0;
  }

  void safeDeleteShader (unsigned int& id)
  {
    if (id > 0 && fun->glIsShader (id) == GL_TRUE)
    {
      fun->glDeleteShader (id);
    }
    id = 0;
  }

  void safeDeleteProgram (unsigned int& id)
  {
    if (id > 0 && fun->glIsProgram (id) == GL_TRUE)
    {
      GLsizei numShaders;
      GLuint  shaderIds[2];

      fun->glGetAttachedShaders (id, 2, &numShaders, shaderIds);

      for (GLsizei i = 0; i < numShaders; i++)
      {
        OpenGL::safeDeleteShader (shaderIds[i]);
      }
      fun->glDeleteProgram (id);
    }
    id = 0;
  }

  unsigned int loadProgram (const char* vertexShader, const char* fragmentShader,
                            bool loadGeometryShader)
  {
    auto showInfoLog = [](GLuint id) {
      const int maxLogLength = 1000;
      char      logBuffer[maxLogLength];
      GLsizei   logLength;
      fun->glGetShaderInfoLog (id, maxLogLength, &logLength, logBuffer);
      if (logLength > 0)
      {
        DILAY_WARN ("%s", logBuffer)
      }
    };

    auto compileShader = [&showInfoLog](GLenum shaderType, const char* shaderSource) -> GLuint {
      GLuint shaderId = fun->glCreateShader (shaderType);
      fun->glShaderSource (shaderId, 1, &shaderSource, NULL);
      fun->glCompileShader (shaderId);

      GLint status;
      fun->glGetShaderiv (shaderId, GL_COMPILE_STATUS, &status);
      if (status == GL_FALSE)
      {
        showInfoLog (shaderId);
        DILAY_PANIC ("can not compile shader: see info log above")
      }
      return shaderId;
    };

    GLuint programId = fun->glCreateProgram ();
    GLuint vsId = compileShader (GL_VERTEX_SHADER, vertexShader);
    GLuint fsId = compileShader (GL_FRAGMENT_SHADER, fragmentShader);
    GLuint gmId = 0;

    fun->glAttachShader (programId, vsId);
    fun->glAttachShader (programId, fsId);

    if (loadGeometryShader)
    {
      assert (OpenGL::supportsGeometryShader ());

      gmId = compileShader (GL_GEOMETRY_SHADER_EXT, Shader::geometryShader ());
      fun->glAttachShader (programId, gmId);

      gsFun->glProgramParameteriEXT (programId, GL_GEOMETRY_VERTICES_OUT_EXT, 3);
      gsFun->glProgramParameteriEXT (programId, GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
      gsFun->glProgramParameteriEXT (programId, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
    }

    fun->glBindAttribLocation (programId, OpenGL::PositionIndex, "position");
    fun->glBindAttribLocation (programId, OpenGL::NormalIndex, "normal");

    fun->glLinkProgram (programId);

    GLint status;
    fun->glGetProgramiv (programId, GL_LINK_STATUS, &status);

    if (status == GL_FALSE)
    {
      showInfoLog (programId);
      OpenGL::safeDeleteProgram (programId);
      DILAY_PANIC ("can not link shader program: see info log above")
    }
    OpenGL::safeDeleteShader (vsId);
    OpenGL::safeDeleteShader (fsId);
    OpenGL::safeDeleteShader (gmId);
    return programId;
  }

  void clearError ()
  {
    fun->glGetError ();
  }

  void printError ()
  {
    const unsigned int glError = fun->glGetError ();

    switch (glError)
    {
      case GL_NO_ERROR:
        std::cout << "GL_NO_ERROR\n";
        break;
      case GL_INVALID_ENUM:
        std::cout << "GL_INVALID_ENUM\n";
        break;
      case GL_INVALID_VALUE:
        std::cout << "GL_INVALID_VALUE\n";
        break;
      case GL_INVALID_OPERATION:
        std::cout << "GL_INVALID_OPERATION\n";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION\n";
        break;
      case GL_OUT_OF_MEMORY:
        std::cout << "GL_OUT_OF_MEMORY\n";
        break;
      case GL_STACK_UNDERFLOW:
        std::cout << "GL_STACK_UNDERFLOW\n";
        break;
      case GL_STACK_OVERFLOW:
        std::cout << "GL_STACK_OVERFLOW\n";
        break;
      default:
        std::cout << "Unknown OpenGL error '" << glError << "'\n";
        break;
    }
  }
}
