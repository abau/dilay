#include <glm/glm.hpp>
#include "camera.hpp"
#include "color.hpp"
#include "config.hpp"
#include "dimension.hpp"
#include "mesh.hpp"
#include "mesh-util.hpp"
#include "mirror.hpp"
#include "opengl.hpp"
#include "render-mode.hpp"

struct Mirror::Impl {
  Dimension dimension;
  float     mirrorWidth;
  Mesh      mirrorMesh;

  Impl (const Config& config, Dimension d)
    : dimension   (d)
    , mirrorWidth (1.0f)
  {
    this->mirrorMesh = MeshUtil::cube ();
    this->mirrorMesh.renderMode ().constantShading (true);
    this->mirrorMesh.bufferData ();

    this->runFromConfig (config);
  }

  glm::vec3 mirror (const glm::vec3& p) {
    switch (this->dimension) {
      case Dimension::X: return glm::vec3 (-p.x, p.y, p.z);
      case Dimension::Y: return glm::vec3 ( p.x,-p.y, p.z);
      case Dimension::Z: return glm::vec3 ( p.x, p.y,-p.z);
    }
    std::abort ();
  }

  void render (Camera& camera) const {
    const glm::vec3 pos    = camera.position ();
    const float     width2 = this->mirrorWidth * 0.5f;
    const bool      inside = 
        (this->dimension == Dimension::X && glm::abs (pos.x) <= width2)
     || (this->dimension == Dimension::Y && glm::abs (pos.y) <= width2)
     || (this->dimension == Dimension::Z && glm::abs (pos.z) <= width2);

    OpenGL::glClear         (OpenGL::StencilBufferBit ());
    OpenGL::glDepthMask     (false);
    OpenGL::glEnable        (OpenGL::StencilTest ());

    OpenGL::glColorMask     (false, false, false, false);

    OpenGL::glCullFace      (OpenGL::Front ());

    OpenGL::glEnable        (OpenGL::StencilTest ());
    OpenGL::glStencilFunc   (OpenGL::Always (), 1, 255);
    OpenGL::glStencilOp     (OpenGL::Keep (), OpenGL::Replace (), OpenGL::Keep ());

    this->mirrorMesh.render (camera);

    if (inside) {
      OpenGL::glDisable (OpenGL::DepthTest ());
    }
    else {
      OpenGL::glCullFace (OpenGL::Back ());
    }

    OpenGL::glColorMask     (true, true, true, true);

    OpenGL::glStencilFunc   (OpenGL::Equal (), 1, 255);
    OpenGL::glStencilOp     (OpenGL::Keep (), OpenGL::Keep (), OpenGL::Keep ());

    OpenGL::glEnable        (OpenGL::Blend ());
    OpenGL::glBlendEquation (OpenGL::FuncAdd ());
    OpenGL::glBlendFunc     (OpenGL::DstColor (), OpenGL::Zero ());

    this->mirrorMesh.render (camera);

    OpenGL::glDisable       (OpenGL::Blend ());

    if (inside) {
      OpenGL::glCullFace (OpenGL::Back ());
      OpenGL::glEnable   (OpenGL::DepthTest ());
    }
    OpenGL::glDisable   (OpenGL::StencilTest ());
    OpenGL::glDepthMask (true);
  }

  void updateMesh () {
    const float extent = 10000.0f;

    switch (this->dimension) {
      case Dimension::X:
        this->mirrorMesh.scaling (glm::vec3 (this->mirrorWidth, extent, extent));
        break;
      case Dimension::Y:
        this->mirrorMesh.scaling (glm::vec3 (extent, this->mirrorWidth, extent));
        break;
      case Dimension::Z:
        this->mirrorMesh.scaling (glm::vec3 (extent, extent, this->mirrorWidth));
        break;
    }
  }

  void runFromConfig (const Config& config) {
    this->mirrorWidth = config.get <float> ("editor/tool/sculpt/mirror/width");
    this->mirrorMesh.color (config.get <Color> ("editor/tool/sculpt/mirror/color"));
    this->updateMesh ();
  }
};

DELEGATE2_BIG6 (Mirror, const Config&, Dimension)
GETTER_CONST    (Dimension, Mirror, dimension)
SETTER          (Dimension, Mirror, dimension)
DELEGATE1_CONST (glm::vec3, Mirror, mirror, const glm::vec3&)
DELEGATE1_CONST (void     , Mirror, render, Camera&)
DELEGATE1       (void     , Mirror, runFromConfig, const Config&)
