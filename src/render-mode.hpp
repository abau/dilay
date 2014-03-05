#ifndef DILAY_RENDER_MODE
#define DILAY_RENDER_MODE

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
