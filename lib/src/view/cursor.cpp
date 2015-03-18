#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../util.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "mesh.hpp"
#include "render-mode.hpp"
#include "view/cursor.hpp"

struct ViewCursor::Impl {
  Mesh       mesh;
  float     _radius;
  glm::vec3 _normal;
  bool       isEnabled;
  bool       hasInnerRadius;
  float     _innerRadiusFactor;

  static const unsigned int numSectors = 40;

  Impl () 
    : _radius            (0.0f)
    , _normal            (0.0f)
    ,  isEnabled         (false) 
    ,  hasInnerRadius    (false)
    , _innerRadiusFactor (0.0f)
  {}

  float radius () const {
    return this->_radius;
  }

  glm::vec3 position () const {
    return this->mesh.position ();
  }

  const glm::vec3& normal () {
    return this->_normal;
  }

  const Color& color () const {
    return this->mesh.color ();
  }

  float innerRadiusFactor () const {
    assert (this->hasInnerRadius);
    return this->_innerRadiusFactor;
  }

  void radius (float r) {
    this->_radius = r;
    this->update ();
  }

  void position (const glm::vec3& p) { 
    this->mesh.position (p); 
  }

  void normal (const glm::vec3& n) {
    this->_normal   = n;
    const float d   = glm::dot (n, glm::vec3 (0.0f,1.0f,0.0f));
    const float eps = Util::epsilon ();
    if (d >= 1.0f - eps || d <= -1.0f + eps) {
      this->mesh.rotationMatrix (glm::mat4(1.0f));
    }
    else {
      const glm::vec3 axis  = glm::cross   (glm::vec3 (0.0f,1.0f,0.0f),n);
      const float     angle = glm::acos (d);
      this->mesh.rotationMatrix (glm::rotate (glm::mat4(1.0f), angle, axis));
    }
  }

  void color (const Color& color) {
    this->mesh.color (color);
  }

  void enable  () { this->isEnabled = true;  }
  void disable () { this->isEnabled = false; }

  void innerRadiusFactor (float f) {
    assert (this->hasInnerRadius);
    assert (f >= 0.0f);
    assert (f <= 1.0f);
    this->_innerRadiusFactor = f;
    this->update ();
  }

  void render (Camera& camera) const {
    if (this->isEnabled) {
      this->mesh.renderLines (camera);
    }
  }

  void update () {
    this->mesh.resetGeometry ();

    Impl::addCircleMesh (this->mesh, this->radius ());

    if (this->hasInnerRadius) {
      Impl::addCircleMesh (this->mesh, this->radius () * this->innerRadiusFactor ());
    }
    this->mesh.renderMode ().constantShading (true);
    this->mesh.renderMode ().noDepthTest (true);
    this->mesh.bufferData ();
  }

  static void addCircleMesh (Mesh& mesh, float radius) {
    static_assert (Impl::numSectors > 2, "");

          float        sectorStep = 2.0f * M_PI / float (Impl::numSectors);
          float        theta      = 0.0f;
    const unsigned int startIndex = mesh.numVertices ();

    for (unsigned int s = startIndex; s < startIndex + Impl::numSectors; s++) {
      float x = radius * sin (theta);
      float z = radius * cos (theta);

      mesh.addVertex (glm::vec3 (x,0.0f,z));
      if (s > startIndex) {
        mesh.addIndex  (s-1);
        mesh.addIndex  (s);
      }
      theta += sectorStep;
    }
    mesh.addIndex (startIndex);
    mesh.addIndex (startIndex + Impl::numSectors - 1);
  }
};

DELEGATE_BIG6    (ViewCursor)
DELEGATE_CONST   (float           , ViewCursor, radius)
DELEGATE_CONST   (glm::vec3       , ViewCursor, position)
DELEGATE_CONST   (const glm::vec3&, ViewCursor, normal)
DELEGATE_CONST   (const Color&    , ViewCursor, color)
GETTER_CONST     (bool            , ViewCursor, isEnabled)
GETTER_CONST     (bool            , ViewCursor, hasInnerRadius)
DELEGATE_CONST   (float           , ViewCursor, innerRadiusFactor)
DELEGATE1        (void            , ViewCursor, radius, float)
DELEGATE1        (void            , ViewCursor, position, const glm::vec3&)
DELEGATE1        (void            , ViewCursor, normal, const glm::vec3&)
DELEGATE1        (void            , ViewCursor, color, const Color&)
DELEGATE         (void            , ViewCursor, enable)
DELEGATE         (void            , ViewCursor, disable)
SETTER           (bool            , ViewCursor, hasInnerRadius)
DELEGATE1        (void            , ViewCursor, innerRadiusFactor, float)
DELEGATE1_CONST  (void            , ViewCursor, render, Camera&)
