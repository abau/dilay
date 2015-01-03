#include "render-mode.hpp"

namespace RenderModeUtil {

  bool rendersWireframe (RenderMode m) {
    switch (m) {
      case RenderMode::SmoothWireframe:   return true;
      case RenderMode::FlatWireframe:     return true;
      case RenderMode::ConstantWireframe: return true;
      default:                            return false;
    }
  }

  RenderMode nonWireframe (RenderMode m) {
    switch (m) {
      case RenderMode::SmoothWireframe:   return RenderMode::Smooth;
      case RenderMode::FlatWireframe:     return RenderMode::Flat;
      case RenderMode::ConstantWireframe: return RenderMode::Constant;
      default:                            return m;
    }
  }

  RenderMode wireframe (RenderMode m) {
    switch (m) {
      case RenderMode::Smooth:   return RenderMode::SmoothWireframe;
      case RenderMode::Flat:     return RenderMode::FlatWireframe;
      case RenderMode::Constant: return RenderMode::ConstantWireframe;
      default:                   return m;
    }
  }
}
