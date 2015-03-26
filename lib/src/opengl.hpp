#ifndef DILAY_OPENGL
#define DILAY_OPENGL

#include <glm/fwd.hpp>
#include <string>

namespace OpenGL {

  // QT related
  void setDefaultFormat    ();
  void initializeFunctions ();

  // wrappers
  unsigned int Always             ();
  unsigned int ArrayBuffer        ();
  unsigned int Back               ();
  unsigned int Blend              ();
  unsigned int ColorBufferBit     ();
  unsigned int CullFace           ();
  unsigned int CW                 ();
  unsigned int CCW                ();
  unsigned int Decr               ();
  unsigned int DecrWrap           ();
  unsigned int DepthBufferBit     ();
  unsigned int DepthTest          ();
  unsigned int DstColor           ();
  unsigned int ElementArrayBuffer ();
  unsigned int Equal              ();
  unsigned int Fill               ();
  unsigned int Float              ();
  unsigned int Front              ();
  unsigned int FrontAndBack       ();
  unsigned int FuncAdd            ();
  unsigned int Greater            ();
  unsigned int Incr               ();
  unsigned int IncrWrap           ();
  unsigned int Invert             ();
  unsigned int Keep               ();
  unsigned int LEqual             ();
  unsigned int Line               ();
  unsigned int Lines              ();
  unsigned int Never              ();
  unsigned int PolygonOffsetFill  ();
  unsigned int Replace            ();
  unsigned int StaticDraw         ();
  unsigned int StencilBufferBit   ();
  unsigned int StencilTest        ();
  unsigned int Triangles          ();
  unsigned int UnsignedInt        ();
  unsigned int Zero               ();

  void glBindBuffer               (unsigned int, unsigned int);
  void glBlendEquation            (unsigned int);
  void glBlendFunc                (unsigned int, unsigned);
  void glBufferData               (unsigned int, unsigned int, const void*, unsigned int);
  void glClear                    (unsigned int);
  void glClearColor               (float, float, float, float);
  void glClearStencil             (int);
  void glColorMask                (bool, bool, bool, bool);
  void glCullFace                 (unsigned int);
  void glDepthFunc                (unsigned int);
  void glDepthMask                (bool);
  void glDisable                  (unsigned int);
  void glDisableVertexAttribArray (unsigned int);
  void glDrawElements             (unsigned int, unsigned int, unsigned int, const void*);
  void glEnable                   (unsigned int);
  void glEnableVertexAttribArray  (unsigned int);
  void glFrontFace                (unsigned int);
  void glGenBuffers               (unsigned int, unsigned int*);
  int  glGetUniformLocation       (unsigned int, const char*);
  bool glIsBuffer                 (unsigned int);
  bool glIsProgram                (unsigned int);
  void glPolygonMode              (unsigned int, unsigned int);
  void glPolygonOffset            (float, float);
  void glStencilFunc              (unsigned int, int, unsigned int);
  void glStencilOp                (unsigned int, unsigned int, unsigned int);
  void glUniform1f                (int, float);
  void glUniformMatrix3fv         (int, unsigned int, bool, const float*);
  void glUniformMatrix4fv         (int, unsigned int, bool, const float*);
  void glUseProgram               (unsigned int);
  void glVertexAttribPointer      (unsigned int, int, unsigned int, bool, unsigned int, const void*);
  void glViewport                 (unsigned int, unsigned int, unsigned int, unsigned int);

  // utilities
  enum VertexAttributIndex { PositionIndex = 0
                           , NormalIndex   = 1
                           };

  bool         supportsGeometryShader ();
  void         glUniformVec3          (unsigned int, const glm::vec3&);
  void         glUniformVec4          (unsigned int, const glm::vec4&);
  void         safeDeleteBuffer       (unsigned int&);
  void         safeDeleteShader       (unsigned int&);
  void         safeDeleteProgram      (unsigned int&);
  unsigned int loadProgram            (const char*, const char*, bool);
}

#endif
