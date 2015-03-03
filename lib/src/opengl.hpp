#ifndef DILAY_OPENGL
#define DILAY_OPENGL

#include <glm/fwd.hpp>
#include <string>

namespace OpenGL {

  // QT related
  void setDefaultFormat    ();
  void initializeFunctions ();

  // wrappers
  unsigned int ArrayBuffer        ();
  unsigned int Back               ();
  unsigned int ColorBufferBit     ();
  unsigned int CullFace           ();
  unsigned int CW                 ();
  unsigned int CCW                ();
  unsigned int DepthBufferBit     ();
  unsigned int DepthTest          ();
  unsigned int ElementArrayBuffer ();
  unsigned int Fill               ();
  unsigned int Float              ();
  unsigned int Front              ();
  unsigned int FrontAndBack       ();
  unsigned int LEqual             ();
  unsigned int Line               ();
  unsigned int Lines              ();
  unsigned int PolygonOffsetFill  ();
  unsigned int StaticDraw         ();
  unsigned int StencilBufferBit   ();
  unsigned int Triangles          ();
  unsigned int UnsignedInt        ();

  void glBindBuffer               (unsigned int, unsigned int);
  void glBufferData               (unsigned int, unsigned int, const void*, unsigned int);
  void glClear                    (unsigned int);
  void glClearColor               (float, float, float, float);
  void glCullFace                 (unsigned int);
  void glDepthFunc                (unsigned int);
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
  void glUniform1f                (int, float);
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
