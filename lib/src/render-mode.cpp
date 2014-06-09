#include "render-mode.hpp"

RenderMode RenderModeUtil :: toggle (RenderMode renderMode) {
  if (renderMode == RenderMode::SmoothShaded)
    return RenderMode::Wireframe;
  else if (renderMode == RenderMode::Wireframe)
    return RenderMode::FlatShaded;
  else
    return RenderMode::SmoothShaded;
}
