#ifndef RENDERMODE
#define RENDERMODE

enum RenderMode { RenderSolid, RenderWireframe };

class RenderModeUtil {
  public:
    static RenderMode toggle (RenderMode renderMode) {
      if (renderMode == RenderSolid)
        return RenderWireframe;
      else 
        return RenderSolid;
    }
};

#endif
