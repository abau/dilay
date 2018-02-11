/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SHADER
#define DILAY_SHADER

namespace Shader
{
  const char* smoothVertexShader ();
  const char* smoothFragmentShader ();
  const char* smoothWireframeFragmentShader ();

  const char* flatVertexShader ();
  const char* flatFragmentShader ();
  const char* flatWireframeFragmentShader ();

  const char* constantVertexShader ();
  const char* constantFragmentShader ();
  const char* constantWireframeFragmentShader ();
  const char* geometryShader ();
};

#endif
