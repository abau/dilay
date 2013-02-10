#ifndef RENDERMODE
#define RENDERMODE

enum RenderMode { RenderSolid = 0, RenderWireframe = 1}; // c.f. numRenderModes

class RenderModeUtil {
  public:
    static const int numRenderModes = 2;

    static RenderMode toggle (RenderMode renderMode) {
      if (renderMode == RenderSolid)
        return RenderWireframe;
      else 
        return RenderSolid;
    }
};

#endif
