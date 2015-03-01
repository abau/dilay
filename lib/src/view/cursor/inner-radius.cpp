#include <glm/glm.hpp>
#include "camera.hpp"
#include "mesh.hpp"
#include "render-flags.hpp"
#include "render-mode.hpp"
#include "view/cursor/inner-radius.hpp"

struct ViewCursorInnerRadius::Impl {
  ViewCursorInnerRadius* self;
  Mesh                   mesh;
  float                 _innerRadiusFactor;

  Impl (ViewCursorInnerRadius* s) 
    : self               (s)
    , _innerRadiusFactor (1.0f)
  {
    this->self->updateCircleGeometry (this->mesh, this->_innerRadiusFactor);
  }

  float innerRadiusFactor () const {
    return this->_innerRadiusFactor;
  }

  float innerRadius () const {
    return this->mesh.scaling ().x;
  }

  void innerRadiusFactor (float f) {
    this->_innerRadiusFactor = f;
    this->mesh.scaling (glm::vec3 (f * this->self->radius ()));
  }

  void runUpdate () {
    this->innerRadiusFactor (this->_innerRadiusFactor);

    this->mesh.color          (this->self->color          ());
    this->mesh.position       (this->self->position       ());
    this->mesh.rotationMatrix (this->self->rotationMatrix ());
  }

  void runRender (Camera& camera) const {
    this->mesh.renderLines (camera, RenderFlags::NoDepthTest ());
  }
};

DELEGATE_BIG6_BASE (ViewCursorInnerRadius, (), (this), ViewCursor, ())
DELEGATE_CONST  (float, ViewCursorInnerRadius, innerRadiusFactor)
DELEGATE1       (void , ViewCursorInnerRadius, innerRadiusFactor, float)
DELEGATE        (void , ViewCursorInnerRadius, runUpdate)
DELEGATE1_CONST (void , ViewCursorInnerRadius, runRender, Camera&)
