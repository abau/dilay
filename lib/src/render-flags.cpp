#include "render-flags.hpp"

void RenderFlags :: noZoom      (bool v) {        this->flags.set <0> (v); }
bool RenderFlags :: noZoom      () const { return this->flags.get <0> ( ); }
void RenderFlags :: noDepthTest (bool v) {        this->flags.set <1> (v); }
bool RenderFlags :: noDepthTest () const { return this->flags.get <1> ( ); }

RenderFlags RenderFlags :: NoZoom () {
  RenderFlags flags;
  flags.noZoom (true);
  return flags;
}

RenderFlags RenderFlags :: NoDepthTest () {
  RenderFlags flags;
  flags.noDepthTest (true);
  return flags;
}
