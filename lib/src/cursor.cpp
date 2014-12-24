#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.hpp"
#include "color.hpp"
#include "cursor.hpp"
#include "mesh.hpp"
#include "opengl.hpp"
#include "render-mode.hpp"
#include "renderer.hpp"
#include "util.hpp"

struct Cursor::Impl {
  Mesh          mesh;
  float         radius;
  unsigned int  sectors;
  bool          isEnabled;

  Impl (float r) : radius (r), sectors (40), isEnabled (true) {
    this->updateGeometry ();
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
    this->mesh.renderMode (RenderMode::Wireframe);
    this->mesh.bufferData ();
  }

  void render (const Camera& camera) {
    if (this->isEnabled) {
      this->mesh.renderBegin (camera);

      OpenGL::glDisable (OpenGL::DepthTest ()); 

      camera.renderer ().setColor3 (Color (1.0f, 0.0f, 0.0f));
      OpenGL::glDrawElements ( OpenGL::Lines (), this->mesh.numIndices ()
                             , OpenGL::UnsignedInt (), (void*)0 );

      OpenGL::glEnable (OpenGL::DepthTest ()); 

      this->mesh.renderEnd ();
    }
  }

  void  enable    ()       { this->isEnabled = true;  }
  void  disable   ()       { this->isEnabled = false; }
};

DELEGATE1_BIG6 (Cursor, float)
SETTER       (float, Cursor, radius)
DELEGATE1    (void,  Cursor, position, const glm::vec3&)
DELEGATE1    (void,  Cursor, normal, const glm::vec3&)
DELEGATE     (void,  Cursor, updateGeometry)
DELEGATE1    (void,  Cursor, render, const Camera&)
DELEGATE     (void,  Cursor, enable)
DELEGATE     (void,  Cursor, disable)
GETTER_CONST (bool,  Cursor, isEnabled)
GETTER_CONST (float, Cursor, radius)
