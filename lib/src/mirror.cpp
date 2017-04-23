/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "camera.hpp"
#include "color.hpp"
#include "config.hpp"
#include "dimension.hpp"
#include "mesh-util.hpp"
#include "mesh.hpp"
#include "mirror.hpp"
#include "opengl.hpp"
#include "primitive/plane.hpp"
#include "render-mode.hpp"

struct Mirror::Impl
{
  Dimension                  _dimension;
  float                      width;
  Mesh                       mirrorMesh;
  std::unique_ptr<PrimPlane> _plane;

  Impl (const Config& config, Dimension d)
    : _dimension (d)
    , width (1.0f)
  {
    this->mirrorMesh = MeshUtil::cube ();
    this->mirrorMesh.renderMode ().constantShading (true);
    this->mirrorMesh.bufferData ();

    this->runFromConfig (config);
    this->dimension (d);
  }

  Dimension dimension () const
  {
    return this->_dimension;
  }
  void dimension (Dimension d)
  {
    this->_dimension = d;
    this->_plane = std::make_unique<PrimPlane> (glm::vec3 (0.0f), DimensionUtil::vector (d));
  }

  const PrimPlane& plane () const
  {
    return *this->_plane;
  }
  void render (Camera& camera) const
  {
    const glm::vec3 pos = camera.position ();
    const float     width2 = this->width * 0.5f;
    const bool      inside = (this->_dimension == Dimension::X && glm::abs (pos.x) <= width2) ||
                        (this->_dimension == Dimension::Y && glm::abs (pos.y) <= width2) ||
                        (this->_dimension == Dimension::Z && glm::abs (pos.z) <= width2);

    OpenGL::glClear (OpenGL::StencilBufferBit ());
    OpenGL::glDepthMask (false);
    OpenGL::glEnable (OpenGL::StencilTest ());

    OpenGL::glColorMask (false, false, false, false);

    OpenGL::glCullFace (OpenGL::Front ());

    OpenGL::glEnable (OpenGL::StencilTest ());
    OpenGL::glStencilFunc (OpenGL::Always (), 1, 255);
    OpenGL::glStencilOp (OpenGL::Keep (), OpenGL::Replace (), OpenGL::Keep ());

    this->mirrorMesh.render (camera);

    if (inside)
    {
      OpenGL::glDisable (OpenGL::DepthTest ());
    }
    else
    {
      OpenGL::glCullFace (OpenGL::Back ());
    }

    OpenGL::glColorMask (true, true, true, true);

    OpenGL::glStencilFunc (OpenGL::Equal (), 1, 255);
    OpenGL::glStencilOp (OpenGL::Keep (), OpenGL::Keep (), OpenGL::Keep ());

    OpenGL::glEnable (OpenGL::Blend ());
    OpenGL::glBlendEquation (OpenGL::FuncAdd ());
    OpenGL::glBlendFunc (OpenGL::DstColor (), OpenGL::Zero ());

    this->mirrorMesh.render (camera);

    OpenGL::glDisable (OpenGL::Blend ());

    if (inside)
    {
      OpenGL::glCullFace (OpenGL::Back ());
      OpenGL::glEnable (OpenGL::DepthTest ());
    }
    OpenGL::glDisable (OpenGL::StencilTest ());
    OpenGL::glDepthMask (true);
  }

  void updateMesh ()
  {
    const float extent = 1000.0f;

    switch (this->_dimension)
    {
      case Dimension::X:
        this->mirrorMesh.scaling (glm::vec3 (this->width, extent, extent));
        break;

      case Dimension::Y:
        this->mirrorMesh.scaling (glm::vec3 (extent, this->width, extent));
        break;

      case Dimension::Z:
        this->mirrorMesh.scaling (glm::vec3 (extent, extent, this->width));
        break;
    }
  }

  void runFromConfig (const Config& config)
  {
    this->width = config.get<float> ("editor/tool/sculpt/mirror/width");
    this->mirrorMesh.color (config.get<Color> ("editor/tool/sculpt/mirror/color"));
    this->updateMesh ();
  }
};

DELEGATE2_BIG2 (Mirror, const Config&, Dimension)
DELEGATE_CONST (Dimension, Mirror, dimension)
GETTER_CONST (float, Mirror, width)
DELEGATE1 (void, Mirror, dimension, Dimension)
DELEGATE_CONST (const PrimPlane&, Mirror, plane)
DELEGATE1_CONST (void, Mirror, render, Camera&)
DELEGATE1 (void, Mirror, runFromConfig, const Config&)
