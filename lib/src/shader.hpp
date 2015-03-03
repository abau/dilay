#ifndef DILAY_SHADER
#define DILAY_SHADER

namespace Shader {
  const char* smoothVertexShader              ();
  const char* smoothFragmentShader            ();
  const char* smoothWireframeFragmentShader   ();

  const char* flatVertexShader                ();
  const char* flatFragmentShader              ();
  const char* flatWireframeFragmentShader     ();

  const char* constantVertexShader            ();
  const char* constantFragmentShader          ();
  const char* constantWireframeFragmentShader ();
  const char* geometryShader                  ();
};

#endif
