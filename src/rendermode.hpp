#ifndef DILAY_RENDERMODE
#define DILAY_RENDERMODE

enum class RenderMode 
  { Smooth    = 0 // c.f. numRenderModes
  , Wireframe = 1
  , Flat      = 2
  }; 

namespace RenderModeUtil {
  const unsigned int numRenderModes = 3;
 
  RenderMode toggle (RenderMode);
}

#endif
