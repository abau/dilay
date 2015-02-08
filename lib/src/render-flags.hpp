#ifndef DILAY_RENDER_FLAGS
#define DILAY_RENDER_FLAGS

#include "bitset.hpp"

class RenderFlags {
  public:
    void noZoom      (bool);
    bool noZoom      () const;
    void noDepthTest (bool);
    bool noDepthTest () const;

    static RenderFlags NoZoom      ();
    static RenderFlags NoDepthTest ();

  private:
    Bitset <int> flags;
};

#endif
