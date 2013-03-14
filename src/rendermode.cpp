#include "rendermode.hpp"

RenderMode RenderModeUtil :: toggle (RenderMode renderMode) {
  if (renderMode == RenderSmooth)
    return RenderWireframe;
  else if (renderMode == RenderWireframe)
    return RenderFlat;
  else
    return RenderSmooth;
}
