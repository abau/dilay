#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include "ray.hpp"

#ifndef CAMERA
#define CAMERA

class Camera {
  public:
                      Camera                ();
    void              initialize            ();

    int               resolutionWidth       () { return this->_resolutionWidth; }
    int               resolutionHeight      () { return this->_resolutionHeight; }

    void              updateResolution      (int, int);
    void              modelViewProjection   (const glm::mat4&) const;
    void              rotationProjection    () const;

    void              stepAlongGaze         (bool);
    void              verticalRotation      (int);
    void              horizontalRotation    (int);
    Ray               getRay                (unsigned int,unsigned int) const;
    void              updateProjection      (unsigned int,unsigned int
                                            ,unsigned int,unsigned int);
    void              updateProjection      ();

  private:
    void              updateView            ();
    glm::vec3         eyePoint              () const;

    float             _gazeStepSize;
    float             _verticalRotationAngle;
    float             _horizontalRotationAngle;

    glm::vec3         _gazePoint;
    glm::vec3         _toEyePoint;
    glm::vec3         _up;
    glm::vec3         _right;
    glm::mat4         _projection;
    glm::mat4         _view;
    glm::mat4         _viewRotation;
    int               _resolutionWidth;
    int               _resolutionHeight;
    float             _nearClipping;
    float             _farClipping;
};

#endif

