/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "camera.hpp"
#include "color.hpp"
#include "mesh.hpp"
#include "mesh-util.hpp"
#include "opengl.hpp"
#include "render-mode.hpp"
#include "view/cursor.hpp"

struct ViewCursor::Impl {
  Mesh       radiusMesh;
  float     _radius;
  bool       isEnabled;

  static const unsigned int numSectors = 40;

  Impl () 
    : _radius            (0.0f)
    ,  isEnabled         (false) 
  {
    this->radiusMesh = MeshUtil::icosphere (2);
    this->radiusMesh.renderMode ().constantShading (true);
    this->radiusMesh.bufferData ();
  }

  float radius () const {
    return this->_radius;
  }

  glm::vec3 position () const {
    return this->radiusMesh.position ();
  }

  const Color& color () const {
    return this->radiusMesh.color ();
  }

  void radius (float r) {
    this->_radius = r;
    this->update ();
  }

  void position (const glm::vec3& p) { 
    this->radiusMesh.position (p); 
  }

  void color (const Color& color) {
    this->radiusMesh.color (color);
  }

  void enable  () { this->isEnabled = true;  }
  void disable () { this->isEnabled = false; }

  void render (Camera& camera) const {
    if (this->isEnabled) {
      OpenGL::glClear         (OpenGL::StencilBufferBit ());

      OpenGL::glDepthMask     (false);
      OpenGL::glColorMask     (false, false, false, false);
      OpenGL::glEnable        (OpenGL::StencilTest ());

      OpenGL::glCullFace      (OpenGL::Front ());

      OpenGL::glEnable        (OpenGL::StencilTest ());
      OpenGL::glStencilFunc   (OpenGL::Always (), 1, 255);
      OpenGL::glStencilOp     (OpenGL::Keep (), OpenGL::Replace (), OpenGL::Keep ());

      this->radiusMesh.render (camera);

      OpenGL::glCullFace      (OpenGL::Back ());
      OpenGL::glColorMask     (true, true, true, true);

      OpenGL::glStencilFunc   (OpenGL::Equal (), 1, 255);
      OpenGL::glStencilOp     (OpenGL::Keep (), OpenGL::Keep (), OpenGL::Keep ());

      OpenGL::glEnable        (OpenGL::Blend ());
      OpenGL::glBlendEquation (OpenGL::FuncAdd ());
      OpenGL::glBlendFunc     (OpenGL::DstColor (), OpenGL::Zero ());

      this->radiusMesh.render (camera);

      OpenGL::glDisable       (OpenGL::Blend ());
      OpenGL::glDisable       (OpenGL::StencilTest ());
      OpenGL::glDepthMask     (true);
    }
  }

  void update () {
    this->radiusMesh.scaling (glm::vec3 (this->radius ()));
  }
};

DELEGATE_BIG6    (ViewCursor)
DELEGATE_CONST   (float           , ViewCursor, radius)
DELEGATE_CONST   (glm::vec3       , ViewCursor, position)
DELEGATE_CONST   (const Color&    , ViewCursor, color)
GETTER_CONST     (bool            , ViewCursor, isEnabled)
DELEGATE1        (void            , ViewCursor, radius, float)
DELEGATE1        (void            , ViewCursor, position, const glm::vec3&)
DELEGATE1        (void            , ViewCursor, color, const Color&)
DELEGATE         (void            , ViewCursor, enable)
DELEGATE         (void            , ViewCursor, disable)
DELEGATE1_CONST  (void            , ViewCursor, render, Camera&)
