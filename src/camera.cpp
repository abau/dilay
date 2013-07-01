#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_major_storage.hpp>
#include "camera.hpp"
#include "opengl-util.hpp"
#include "ray.hpp"
#include "renderer.hpp"
#include "macro.hpp"

struct Camera::Impl {
  float        gazeStepSize;

  glm::vec3    gazePoint;
  glm::vec3    toEyePoint;
  glm::vec3    up;
  glm::vec3    right;
  glm::mat4x4  projection;
  glm::mat4x4  view;
  glm::mat4x4  viewRotation;
  glm::uvec2   resolution;
  float        nearClipping;
  float        farClipping;

  Impl () : gazeStepSize (1.0f)
          , resolution   (1024,800)
          , nearClipping (0.1f)
          , farClipping  (1000.0f) {


    this->gazePoint  = glm::vec3 (0.0f,0.0f,0.0f);
    this->up         = glm::vec3 (0.0f,1.0f,0.0f);
    this->toEyePoint = glm::vec3 (3.0f,3.0f,3.0f);

    this->right      = glm::normalize ( glm::cross (this->up, this->toEyePoint) );
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

  void modelViewProjection (const glm::mat4x4& model) const {
    glm::mat4x4 mvp = this->projection * this->view * model;
    Renderer :: setMvp (&mvp[0][0]);
  }

  void rotationProjection () const {
    glm::mat4x4 mvp = this->projection * this->viewRotation;
    Renderer :: setMvp (&mvp[0][0]);
  }

  void stepAlongGaze (bool forward) {
    float d = glm::length (this->toEyePoint);
    if (forward && d > 1.0f)
      this->toEyePoint *= glm::vec3 (0.5f);
    else if (!forward)
      this->toEyePoint *= glm::vec3 (2.0f);
    this->updateView ();
  }

  void verticalRotation (float angle) {
    glm::fquat q      = glm::angleAxis (angle,this->up);
    this->right       = glm::rotate (q, this->right);
    this->toEyePoint  = glm::rotate (q, this->toEyePoint);
    this->updateView ();
  }

  void horizontalRotation (float angle) {
    glm::fquat q      = glm::angleAxis (angle,this->right);
    this->toEyePoint = glm::rotate (q, this->toEyePoint);
    this->updateView ();
  }

  glm::vec3 toWorld (const glm::uvec2& p) const {
    return glm::unProject ( glm::vec3 (float (p.x), float (p.y), 0.0f)
                          , this->view, this->projection
                          , glm::vec4 ( 0, 0
                                      , this->resolution.x
                                      , this->resolution.y)
                          );
  }

  Ray getRay (const glm::uvec2& p) const {
    glm::vec3 w   = this->toWorld  (p);
    glm::vec3 eye = this->eyePoint ();
    return Ray (eye, glm::normalize (w - eye));
  }

  Ray getRayInvY (const glm::uvec2& p) const {
    return this->getRay (glm::uvec2 (p.x, this->resolution.y - p.y));
  }

  void updateProjection (const glm::uvec2& p, const glm::uvec2& dim) {
    glViewport (p.x, p.y, dim.x, dim.y);
    this->projection = glm::perspective ( 
        45.0f
      , float (dim.x) / float (dim.y)
      , this->nearClipping, this->farClipping);
  }

  void updateProjection () {
    this->updateProjection (glm::uvec2(0,0),this->resolution);
  }

  void updateView () {
    this->view = glm::lookAt ( this->eyePoint (), this->gazePoint, this->up );

    this->viewRotation = glm::lookAt ( glm::normalize (this->toEyePoint)
                                     , glm::vec3 (0.0f)
                                     , this->up );
  }

  glm::vec3 eyePoint () const {
    return this->gazePoint + this->toEyePoint;
  }
};

DELEGATE_BIG4 (Camera)

DELEGATE        (void              , Camera, initialize)
GETTER          (const glm::uvec2& , Camera, resolution)
GETTER          (const glm::vec3&  , Camera, gazePoint)
GETTER          (const glm::vec3&  , Camera, toEyePoint)
GETTER          (const glm::vec3&  , Camera, up)
GETTER          (const glm::vec3&  , Camera, right)
GETTER          (const glm::mat4x4&, Camera, view)
GETTER          (const glm::mat4x4&, Camera, viewRotation)
DELEGATE_CONST  (glm::vec3         , Camera, position)
DELEGATE_CONST  (glm::mat4x4       , Camera, world)

DELEGATE1       (void     , Camera, updateResolution, const glm::uvec2&) 
DELEGATE1_CONST (void     , Camera, modelViewProjection, const glm::mat4x4&) 
DELEGATE_CONST  (void     , Camera, rotationProjection) 
DELEGATE1       (void     , Camera, stepAlongGaze, bool) 
DELEGATE1       (void     , Camera, verticalRotation, float) 
DELEGATE1       (void     , Camera, horizontalRotation, float) 
DELEGATE1_CONST (glm::vec3, Camera, toWorld, const glm::uvec2&)
DELEGATE1_CONST (Ray      , Camera, getRay, const glm::uvec2&)
DELEGATE1_CONST (Ray      , Camera, getRayInvY, const glm::uvec2&)
DELEGATE2       (void     , Camera, updateProjection, const glm::uvec2&, const glm::uvec2&)
DELEGATE        (void     , Camera, updateProjection)
