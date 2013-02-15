#ifndef RENDERMODE
#define RENDERMODE

enum RenderMode { RenderSolid     = 0 // c.f. numRenderModes
                , RenderWireframe = 1
                , RenderFlat      = 2
                }; 

class RenderModeUtil {
  public:
    static const int numRenderModes = 3;

    static RenderMode toggle (RenderMode renderMode) {
      if (renderMode == RenderSolid)
        return RenderWireframe;
      else if (renderMode == RenderWireframe)
        return RenderFlat;
      else
        return RenderSolid;
    }
};

#endif
