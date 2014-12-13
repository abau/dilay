#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_major_storage.hpp>
#include <glm/gtx/quaternion.hpp>
#include "camera.hpp"
#include "config.hpp"
#include "dimension.hpp"
#include "opengl-util.hpp"
#include "primitive/ray.hpp"
#include "renderer.hpp"

struct Camera::Impl {
  Camera*      self;
  float        gazeStepSize;

  glm::vec3    gazePoint;
  glm::vec3    toEyePoint;
  glm::vec3    up;
  glm::vec3    right;
  glm::mat4x4  projection;
  glm::mat4x4  view;
  glm::mat4x4  viewNoZoom;
  glm::uvec2   resolution;
  float        nearClipping;
  float        farClipping;

  Impl (Camera* s) 
    : self         (s)
    , gazeStepSize ( Config::get <float> ("/config/editor/camera/gaze-step-size") )
    , resolution   ( Config::get <int>   ("/config/editor/camera/initial-resolution/width") 
                   , Config::get <int>   ("/config/editor/camera/initial-resolution/height") )
    , nearClipping ( Config::get <float> ("/config/editor/camera/near-clipping") )
    , farClipping  ( Config::get <float> ("/config/editor/camera/far-clipping") ) {

    this->set ( Config::get <glm::vec3> ("/config/editor/camera/gaze-point")
              , Config::get <glm::vec3> ("/config/editor/camera/eye-point")
              , Config::get <glm::vec3> ("/config/editor/camera/up")
              , false);
  }

  glm::vec3 position () const { return this->gazePoint + this->toEyePoint; }

  glm::mat4x4 world () const {
    glm::vec3   x = this->right;
    glm::vec3   z = glm::normalize (this->toEyePoint);
    glm::vec3   y = glm::cross (z,x);
    glm::vec3   p = this->position ();

    return glm::colMajor4 ( glm::vec4 (x,0.0f)
                          , glm::vec4 (y,0.0f)
                          , glm::vec4 (z,0.0f)
                          , glm::vec4 (p,1.0f));
  }

  void initialize () {
    this->updateView ();
    this->updateProjection ();
  }

  void updateResolution (const glm::uvec2& dimension) {
    this->resolution = dimension;
    this->updateProjection ();
  }

  glm::mat4x4 modelViewProjection (const glm::mat4x4& model, bool noZoom) const {
    return noZoom ? this->projection * this->viewNoZoom * model
                  : this->projection * this->view       * model;
  }

  void setModelViewProjection (const glm::mat4x4& model, bool noZoom) const {
    glm::mat4x4 mvp = this->modelViewProjection (model, noZoom);
    Renderer :: setMvp   (&mvp  [0][0]);
    Renderer :: setModel (&model[0][0]);
  }

  void set (const glm::vec3& g, const glm::vec3& e, const glm::vec3& u, bool update = true) {
    this->gazePoint  = g;
    this->toEyePoint = e;
    this->up         = u;
    this->right      = glm::normalize ( glm::cross (this->up, this->toEyePoint) );
    if (update) {
      this->updateView ();
    }
  }

  void setGaze (const glm::vec3& g) {
    this->gazePoint  = g;
    this->updateView ();
  }

  void stepAlongGaze (float factor) {
    float d = glm::length (this->toEyePoint);
    if (factor < 1.0f && d > 1.0f)
      this->toEyePoint *= glm::vec3 (factor);
    else if (factor > 1.0f)
      this->toEyePoint *= glm::vec3 (factor);
    this->updateView ();
  }

  void verticalRotation (float angle) {
    glm::quat q       = glm::angleAxis (angle, this->up);
    this->right       = glm::rotate (q, this->right);
    this->toEyePoint  = glm::rotate (q, this->toEyePoint);
    this->updateView ();
  }

  void horizontalRotation (float angle) {
    glm::quat q      = glm::angleAxis (angle, this->right);
    this->toEyePoint = glm::rotate (q, this->toEyePoint);

    if (glm::dot ( glm::cross (this->up, this->toEyePoint), this->right ) < 0) {
      this->up = -this->up;
    }
    this->updateView ();
  }

  glm::vec4 viewport () const {
    return glm::vec4 (0, 0, this->resolution.x, this->resolution.y);
  }

  glm::ivec2 fromWorld (const glm::vec3& p, const glm::mat4x4& model, bool noZoom) const {
    glm::mat4x4 mv = noZoom ? this->viewNoZoom * model
                            : this->view       * model;
    glm::vec3 w = glm::project (p, mv, this->projection, this->viewport ());
    return glm::ivec2 (int (w.x), int (resolution.y - w.y));
  }

  glm::vec3 toWorld (const glm::ivec2& p, float z = 0.0f) const {
    float invY  = this->resolution.y - float (p.y);
    float normZ = z / this->farClipping;
    return glm::unProject ( glm::vec3 (float (p.x), invY, normZ)
                          , this->view, this->projection, this->viewport ());
  }

  PrimRay ray (const glm::ivec2& p) const {
    glm::vec3 w   = this->toWorld  (p);
    glm::vec3 eye = this->eyePoint ();
    return PrimRay (eye, glm::normalize (w - eye));
  }

  void updateProjection () {
    glViewport (0, 0, this->resolution.x, this->resolution.y);
    this->projection = glm::perspective ( 
        glm::radians (45.0f)
      , float (this->resolution.x) / float (this->resolution.y)
      , this->nearClipping, this->farClipping);
  }

  void updateView () {
    this->view = glm::lookAt ( this->eyePoint (), this->gazePoint, this->up );

    this->viewNoZoom = glm::lookAt ( glm::normalize (this->toEyePoint)
                                   , glm::vec3 (0.0f)
                                   , this->up );
    Renderer::setEyePoint  (this->eyePoint ());
    Renderer::updateLights (*this->self);
  }

  glm::vec3 eyePoint () const {
    return this->gazePoint + this->toEyePoint;
  }

  Dimension primaryDimension () const {
    glm::vec3 t = glm::abs (this->toEyePoint);
    if (t.x > t.y && t.x > t.z) {
      return Dimension::X;
    }
    else if (t.y > t.z) {
      return Dimension::Y;
    }
    return Dimension::Z;
  }
};

DELEGATE_BIG6_SELF (Camera)

DELEGATE        (void              , Camera, initialize)
GETTER_CONST    (const glm::uvec2& , Camera, resolution)
GETTER_CONST    (const glm::vec3&  , Camera, gazePoint)
GETTER_CONST    (const glm::vec3&  , Camera, toEyePoint)
GETTER_CONST    (const glm::vec3&  , Camera, up)
GETTER_CONST    (const glm::vec3&  , Camera, right)
GETTER_CONST    (const glm::mat4x4&, Camera, view)
GETTER_CONST    (const glm::mat4x4&, Camera, viewNoZoom)
DELEGATE_CONST  (glm::vec3         , Camera, position)
DELEGATE_CONST  (glm::mat4x4       , Camera, world)

DELEGATE1       (void       , Camera, updateResolution, const glm::uvec2&) 
DELEGATE2_CONST (glm::mat4x4, Camera, modelViewProjection, const glm::mat4x4&, bool) 
DELEGATE2_CONST (void       , Camera, setModelViewProjection, const glm::mat4x4&, bool) 
DELEGATE3       (void       , Camera, set, const glm::vec3&, const glm::vec3&, const glm::vec3&)
DELEGATE1       (void       , Camera, setGaze, const glm::vec3&)
DELEGATE1       (void       , Camera, stepAlongGaze, float) 
DELEGATE1       (void       , Camera, verticalRotation, float) 
DELEGATE1       (void       , Camera, horizontalRotation, float) 
DELEGATE3_CONST (glm::ivec2 , Camera, fromWorld, const glm::vec3&, const glm::mat4x4&, bool)
DELEGATE2_CONST (glm::vec3  , Camera, toWorld, const glm::ivec2&, float)
DELEGATE1_CONST (PrimRay    , Camera, ray, const glm::ivec2&)
DELEGATE_CONST  (Dimension  , Camera, primaryDimension)
