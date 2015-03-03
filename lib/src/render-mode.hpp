#ifndef DILAY_RENDER_MODE
#define DILAY_RENDER_MODE

#include "bitset.hpp"

class RenderMode {
  public:
    RenderMode ();
    RenderMode (const RenderMode&);

    bool        smoothShading      () const;
    bool        flatShading        () const;
    bool        constantShading    () const;
    bool        renderWireframe    () const;
    bool        cameraRotationOnly () const;
    bool        noDepthTest        () const;
    const char* vertexShader       () const;
    const char* fragmentShader     () const;

    void        smoothShading      (bool);
    void        flatShading        (bool);
    void        constantShading    (bool);
    void        renderWireframe    (bool);
    void        cameraRotationOnly (bool);
    void        noDepthTest        (bool);

  private:
    Bitset <unsigned int> flags;
};

#endif
