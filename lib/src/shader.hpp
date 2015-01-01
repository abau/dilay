#ifndef DILAY_SHADER
#define DILAY_SHADER

#include <string>

namespace Shader {
  const std::string smoothVertexShader   ();
  const std::string smoothFragmentShader ();

  const std::string flatVertexShader     ();
  const std::string flatFragmentShader   ();

  const std::string constantVertexShader   ();
  const std::string constantFragmentShader ();
};

#endif
