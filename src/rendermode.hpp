#ifndef DILAY_RENDERMODE
#define DILAY_RENDERMODE

enum RenderMode : short 
  { RenderSmooth    = 0 // c.f. numRenderModes
  , RenderWireframe = 1
  , RenderFlat      = 2
  }; 

namespace RenderModeUtil {
  const int numRenderModes = 3;
 
  RenderMode toggle (RenderMode);
}

#endif
