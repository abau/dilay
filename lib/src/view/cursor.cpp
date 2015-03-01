#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../util.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "mesh.hpp"
#include "render-flags.hpp"
#include "render-mode.hpp"
#include "view/cursor.hpp"

struct ViewCursor::Impl {
  ViewCursor* self;
  Mesh        mesh;
  bool        isEnabled;

  static const unsigned int numSectors = 40;

  Impl (ViewCursor* s) 
    : self      (s)
    , isEnabled (true) 
  {
    Impl::updateCircleGeometry (this->mesh, this->radius ());
  }

  float radius () const {
    return this->mesh.scaling ().x;
  }

  const Color&       color          () const { return this->mesh.color          (); }
        glm::vec3    position       () const { return this->mesh.position       (); }
  const glm::mat4x4& rotationMatrix () const { return this->mesh.rotationMatrix (); }

  void radius (float r) {
    this->mesh.scaling (glm::vec3 (r));
    this->self->runUpdate ();
  }

  void color (const Color& color) {
    this->mesh.color (color);
    this->self->runUpdate ();
  }

  void position (const glm::vec3& v) { 
    this->mesh.position (v); 
    this->self->runUpdate ();
  }

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
    this->self->runUpdate ();
  }

  void render (Camera& camera) const {
    if (this->isEnabled) {
      this->mesh.renderLines (camera, RenderFlags::NoDepthTest ());
      this->self->runRender  (camera);
    }
  }

  void enable  () { this->isEnabled = true;  }
  void disable () { this->isEnabled = false; }

  static void updateCircleGeometry (Mesh& mesh, float radius) {
    static_assert (Impl::numSectors > 2, "");
    float sectorStep = 2.0f * M_PI / float (Impl::numSectors);
    float theta      = 0.0f;

    mesh.resetGeometry ();

    for (unsigned int s = 0; s < Impl::numSectors; s++) {
      float x = radius * sin (theta);
      float z = radius * cos (theta);

      mesh.addVertex (glm::vec3 (x,0.0f,z));
      if (s > 0) {
        mesh.addIndex  (s-1);
        mesh.addIndex  (s);
      }
      theta += sectorStep;
    }
    mesh.addIndex   (0);
    mesh.addIndex   (Impl::numSectors-1);
    mesh.renderMode (RenderMode::Constant);
    mesh.bufferData ();
  }
};

DELEGATE_BIG6_SELF (ViewCursor)
DELEGATE_CONST   (float             , ViewCursor, radius)
DELEGATE_CONST   (const Color&      , ViewCursor, color)
DELEGATE_CONST   (glm::vec3         , ViewCursor, position)
DELEGATE_CONST   (const glm::mat4x4&, ViewCursor, rotationMatrix)
DELEGATE1        (void              , ViewCursor, radius, float)
DELEGATE1        (void              , ViewCursor, color, const Color&)
DELEGATE1        (void              , ViewCursor, position, const glm::vec3&)
DELEGATE1        (void              , ViewCursor, normal, const glm::vec3&)
DELEGATE1_CONST  (void              , ViewCursor, render, Camera&)
DELEGATE         (void              , ViewCursor, enable)
DELEGATE         (void              , ViewCursor, disable)
GETTER_CONST     (bool              , ViewCursor, isEnabled)
DELEGATE2_STATIC (void              , ViewCursor, updateCircleGeometry, Mesh&, float)
