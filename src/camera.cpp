#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_major_storage.hpp>
#include "camera.hpp"
#include "opengl-util.hpp"
#include "ray.hpp"
#include "renderer.hpp"
#include "macro.hpp"

struct CameraImpl {
  float        gazeStepSize;
  float        verticalRotationAngle;
  float        horizontalRotationAngle;

  glm::vec3    gazePoint;
  glm::vec3    toEyePoint;
  glm::vec3    up;
  glm::vec3    right;
  glm::mat4x4  projection;
  glm::mat4x4  view;
  glm::mat4x4  viewRotation;
  unsigned int resolutionWidth;
  unsigned int resolutionHeight;
  float        nearClipping;
  float        farClipping;

  CameraImpl () : gazeStepSize            (1.0f)
                , verticalRotationAngle   (0.5f)
                , horizontalRotationAngle (0.5f)
                , resolutionWidth         (1024)
                , resolutionHeight        (800)
                , nearClipping            (0.1f)
                , farClipping             (1000.0f) {


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

  void updateResolution (unsigned int width, unsigned int height) {
    this->resolutionWidth  = width;
    this->resolutionHeight = height;
    this->updateProjection ();
  }

  void modelViewProjection (const glm::mat4x4& model) const {
    glm::mat4x4 mvp = this->projection * this->view * model;
    Renderer :: global ().setMvp (&mvp[0][0]);
  }

  void rotationProjection () const {
    glm::mat4x4 mvp = this->projection * this->viewRotation;
    Renderer :: global ().setMvp (&mvp[0][0]);
  }

  void stepAlongGaze (bool forward) {
    float d = glm::length (this->toEyePoint);
    if (forward && d > 1.0f)
      this->toEyePoint *= glm::vec3 (0.5f);
    else if (!forward)
      this->toEyePoint *= glm::vec3 (2.0f);
    this->updateView ();
  }

  void verticalRotation (int steps) {
    float angle       = this->verticalRotationAngle * float (-steps);

    glm::fquat q      = glm::angleAxis (angle,this->up);
    this->right       = glm::rotate (q, this->right);
    this->toEyePoint  = glm::rotate (q, this->toEyePoint);
    this->updateView ();
  }

  void horizontalRotation (int steps) {
    float angle       = this->horizontalRotationAngle * float (-steps);

    glm::fquat q      = glm::angleAxis (angle,this->right);
    this->toEyePoint = glm::rotate (q, this->toEyePoint);
    this->updateView ();
  }

  Ray getRay (unsigned int x, unsigned int y) const {
    glm::vec3 p = glm::unProject ( glm::vec3 (float (x), float (y), 0.0f)
                                 , this->view, this->projection
                                 , glm::vec4 ( 0, 0
                                             , this->resolutionWidth
                                             , this->resolutionHeight)
                                 );
    glm::vec3 eye = this->eyePoint ();
    return Ray (eye, glm::normalize (p - eye));
  }

  void updateProjection ( unsigned int x, unsigned int y
                        , unsigned int w, unsigned int h) {
    glViewport (x,y,w,h);
    this->projection = glm::perspective ( 
        45.0f
      , float (w) / float (h)
      , this->nearClipping, this->farClipping);
  }

  void updateProjection () {
    this->updateProjection (0,0,this->resolutionWidth,this->resolutionHeight);
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
GETTER          (unsigned int      , Camera, resolutionWidth)
GETTER          (unsigned int      , Camera, resolutionHeight)
GETTER          (const glm::vec3&  , Camera, toEyePoint)
GETTER          (const glm::vec3&  , Camera, up)
GETTER          (const glm::vec3&  , Camera, right)
GETTER          (const glm::mat4x4&, Camera, view)
GETTER          (const glm::mat4x4&, Camera, viewRotation)
DELEGATE_CONST  (glm::vec3         , Camera, position)
DELEGATE_CONST  (glm::mat4x4        , Camera, world)

DELEGATE2       (void , Camera, updateResolution, unsigned int, unsigned int) 
DELEGATE1_CONST (void , Camera, modelViewProjection, const glm::mat4x4&) 
DELEGATE_CONST  (void , Camera, rotationProjection) 
DELEGATE1       (void , Camera, stepAlongGaze, bool) 
DELEGATE1       (void , Camera, verticalRotation, int) 
DELEGATE1       (void , Camera, horizontalRotation, int) 
DELEGATE2_CONST (Ray  , Camera, getRay, unsigned int, unsigned int) 
DELEGATE4       (void , Camera, updateProjection, unsigned int, unsigned int, unsigned int, unsigned int) 
DELEGATE        (void , Camera, updateProjection)
