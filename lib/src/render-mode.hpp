#ifndef DILAY_RENDER_MODE
#define DILAY_RENDER_MODE

enum class RenderMode 
  { SmoothShaded = 0 // c.f. numRenderModes
  , Wireframe    = 1
  , FlatShaded   = 2
  , Color        = 3
  }; 

namespace RenderModeUtil {
  const unsigned int numRenderModes = 4;
 
  RenderMode toggle (RenderMode);
}

#endif
