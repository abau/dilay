#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../util.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "mesh.hpp"
#include "opengl.hpp"
#include "render-mode.hpp"
#include "view/cursor.hpp"

struct ViewCursor::Impl {
  Mesh          mesh;
  float         radius;
  unsigned int  sectors;
  bool          isEnabled;

  Impl (float r, const Color& c) 
    : radius    (r)
    , sectors   (40)
    , isEnabled (true) 
  {
    this->updateGeometry ();
    this->color          (c);
  }

  void position (const glm::vec3& v) { this->mesh.position (v); }

  void normal (const glm::vec3& v) {
    const float d   = glm::dot (v, glm::vec3 (0.0f,1.0f,0.0f));
    const float eps = Util::epsilon ();
    if (d >= 1.0f - eps || d <= -1.0f + eps) {
      this->mesh.rotationMatrix (glm::mat4(1.0f));
    }
    else {
      const glm::vec3 axis  = glm::cross   (glm::vec3 (0.0f,1.0f,0.0f),v);
      const float     angle = glm::acos (d);
      this->mesh.rotationMatrix (glm::rotate (glm::mat4(1.0f), angle, axis));
    }
  }

  void color (const Color& color) {
    this->mesh.color (color);
  }

  void updateGeometry () {
    assert (this->sectors > 2);
    float sectorStep = 2.0f * M_PI / float (this->sectors);
    float theta      = 0.0f;

    this->mesh.resetGeometry ();

    for (unsigned int s = 0; s < this->sectors; s++) {
      float x = this->radius * sin (theta);
      float z = this->radius * cos (theta);

      this->mesh.addVertex (glm::vec3 (x,0.0f,z));
      if (s > 0) {
        this->mesh.addIndex  (s-1);
        this->mesh.addIndex  (s);
      }
      theta += sectorStep;
    }
    this->mesh.addIndex   (0);
    this->mesh.addIndex   (this->sectors-1);
    this->mesh.renderMode (RenderMode::Constant);
    this->mesh.bufferData ();
  }

  void render (const Camera& camera) {
    if (this->isEnabled) {
      this->mesh.renderBegin (camera);

      OpenGL::glDisable      (OpenGL::DepthTest ()); 
      OpenGL::glDrawElements ( OpenGL::Lines (), this->mesh.numIndices ()
                             , OpenGL::UnsignedInt (), (void*)0 );
      OpenGL::glEnable       (OpenGL::DepthTest ()); 

      this->mesh.renderEnd ();
    }
  }

  void  enable    ()       { this->isEnabled = true;  }
  void  disable   ()       { this->isEnabled = false; }
};

DELEGATE2_BIG6 (ViewCursor, float, const Color&)
SETTER       (float, ViewCursor, radius)
DELEGATE1    (void,  ViewCursor, position, const glm::vec3&)
DELEGATE1    (void,  ViewCursor, normal, const glm::vec3&)
DELEGATE1    (void,  ViewCursor, color, const Color&)
DELEGATE     (void,  ViewCursor, updateGeometry)
DELEGATE1    (void,  ViewCursor, render, const Camera&)
DELEGATE     (void,  ViewCursor, enable)
DELEGATE     (void,  ViewCursor, disable)
GETTER_CONST (bool,  ViewCursor, isEnabled)
GETTER_CONST (float, ViewCursor, radius)
