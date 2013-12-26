#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "cursor.hpp"
#include "mesh.hpp"
#include "renderer.hpp"
#include "color.hpp"
#include "util.hpp"

struct Cursor::Impl {
  Mesh          mesh;
  float         radius;
  unsigned int  sectors;
  bool          _isEnabled;

  Impl () : radius (0.2f), sectors (20), _isEnabled (false) {}

  void initialize () {
    assert (this->sectors > 2);
    float sectorStep = 2.0f * M_PI / float (this->sectors);
    float theta      = 0.0f;

    this->mesh.reset ();

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
    this->mesh.bufferData ();
  }

  void setPosition (const glm::vec3& v) { this->mesh.setPosition (v); }

  void setNormal (const glm::vec3& v) {
    float d = glm::dot (v, glm::vec3 (0.0f,1.0f,0.0f));
    if (d >= 1.0f - Util :: epsilon || d <= -1.0f + Util :: epsilon) {
      this->mesh.setRotation (glm::mat4(1.0f));
    }
    else {
      glm::vec3 axis  = glm::cross   (glm::vec3 (0.0f,1.0f,0.0f),v);
      float     angle = glm::degrees (glm::acos (d));
      this->mesh.setRotation (glm::rotate (glm::mat4(1.0f), angle, axis));
    }
  }

  void render () {
    if (this->_isEnabled) {
      this->mesh.renderBegin ();

      glDisable (GL_DEPTH_TEST); 

      Renderer :: global ().setColor3 (Color (1.0f, 0.0f, 0.0f));
      glDrawElements     (GL_LINES, this->mesh.numIndices (), GL_UNSIGNED_INT, (void*)0);

      glEnable (GL_DEPTH_TEST); 

      this->mesh.renderEnd ();
    }
  }

  void  enable    ()       { this->_isEnabled = true;  }
  void  disable   ()       { this->_isEnabled = false; }
  bool  isEnabled () const { return this->_isEnabled;  }
};

DELEGATE_BIG5 (Cursor)

DELEGATE        (void,  Cursor, initialize)
DELEGATE1       (void,  Cursor, setPosition, const glm::vec3&)
DELEGATE1       (void,  Cursor, setNormal, const glm::vec3&)
DELEGATE        (void,  Cursor, render)
DELEGATE        (void,  Cursor, enable)
DELEGATE        (void,  Cursor, disable)
DELEGATE_CONST  (bool,  Cursor, isEnabled)
GETTER_CONST    (float, Cursor, radius)
