#ifndef DILAY_RENDER_MODE
#define DILAY_RENDER_MODE

enum class RenderMode 
  { Smooth            = 0 // c.f. numRenderModes
  , Flat              = 1
  , Constant          = 2
  , SmoothWireframe   = 3
  , FlatWireframe     = 4
  , ConstantWireframe = 5
  }; 

namespace RenderModeUtil {
  const unsigned int numRenderModes = 6;

  bool       rendersWireframe (RenderMode);
  RenderMode nonWireframe     (RenderMode);
  RenderMode wireframe        (RenderMode);
}

#endif
