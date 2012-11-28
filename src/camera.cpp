#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "camera.hpp"

Camera :: Camera () {
  this->_gazeStepSize            = 1.0f;
  this->_verticalRotationAngle   = 0.5f;
  this->_horizontalRotationAngle = 0.5f;
  this->_gazePoint               = glm::vec3 (0.0f);
  this->_toEyePoint              = glm::vec3 (0.0f,0.0f,3.0f);
  this->_up                      = glm::vec3 (0.0f,1.0f,0.0f);
  this->_right                   = glm::vec3 (1.0f,0.0f,0.0f);
  this->_resolutionWidth         = 1024;
  this->_resolutionHeight        = 800;
  this->_nearClipping            = 0.01f;
  this->_farClipping             = 1000.0f;
}

void Camera :: initialize () {
  this->updateView ();
  this->updateProjection ();
}

void Camera :: updateView () {
  this->_eyePoint = this->_gazePoint + this->_toEyePoint;
  this->_view     = glm::lookAt ( this->_eyePoint, this->_gazePoint, this->_up );
}

void Camera :: modelViewProjection (const glm::mat4& model) const {
  glm::mat4 mvp = this->_projection * this->_view * model;
  glUniformMatrix4fv(this->_mvpId, 1, GL_FALSE, &mvp[0][0]);
}

void Camera :: stepAlongGaze (bool forward) {
  float d = glm::length (this->_toEyePoint);
  if (forward && d > 1.0f)
    this->_toEyePoint *= glm::vec3 (0.5f);
  else if (!forward)
    this->_toEyePoint *= glm::vec3 (2.0f);
  this->updateView ();
}

void Camera :: verticalRotation (int steps) {
  float angle     = this->_verticalRotationAngle * float (steps);

  glm::fquat q      = glm::angleAxis (angle,this->_up);
  this->_right      = glm::rotate (q, this->_right);
  this->_toEyePoint = glm::rotate (q, this->_toEyePoint);
  this->updateView ();
}

void Camera :: horizontalRotation (int steps) {
  float angle     = this->_horizontalRotationAngle * float (steps);

  glm::fquat q      = glm::angleAxis (angle,this->_right);
  this->_toEyePoint = glm::rotate (q, this->_toEyePoint);
  this->updateView ();
}

Ray Camera :: getRay (int x, int y) {
  glm::vec3 p = glm::unProject ( glm::vec3 (float (x), float (y), 0.0f)
                               , this->_view, this->_projection
                               , glm::vec4 ( 0, 0
                                           , this->_resolutionWidth
                                           , this->_resolutionHeight)
                               );
  return Ray (this->_eyePoint, glm::normalize (p - this->_eyePoint));
}

void Camera :: updateProjection () {
  glViewport (0,0,this->_resolutionWidth,this->_resolutionHeight);
  this->_projection = glm::perspective ( 
      45.0f
    , float (this->_resolutionWidth) / float (this->_resolutionHeight)
    , this->_nearClipping, this->_farClipping);
}

void Camera :: updateResolution (int width, int height) {
  this->_resolutionWidth  = width;
  this->_resolutionHeight = height;
  this->updateProjection ();
}

