#include "render-mode.hpp"

RenderMode RenderModeUtil :: toggle (RenderMode renderMode) {
  if (renderMode == RenderMode::Smooth)
    return RenderMode::Wireframe;
  else if (renderMode == RenderMode::Wireframe)
    return RenderMode::Flat;
  else
    return RenderMode::Smooth;
}
