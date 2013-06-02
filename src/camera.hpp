#ifndef DILAY_CAMERA
#define DILAY_CAMERA

#include "fwd-glm.hpp"

class Ray;

class Camera {
  public:
          Camera            ();
          Camera            (const Camera&);
    const Camera& operator= (const Camera&);
         ~Camera            ();

    void               initialize            ();

    unsigned int       resolutionWidth       () const;
    unsigned int       resolutionHeight      () const;
    const glm::vec3&   toEyePoint            () const;
    const glm::vec3&   up                    () const;
    const glm::vec3&   right                 () const;
    const glm::mat4x4& view                  () const;
    const glm::mat4x4& viewRotation          () const;
          glm::vec3    position              () const;
          glm::mat4x4  world                 () const;

    void updateResolution      (unsigned int, unsigned int);
    void modelViewProjection   (const glm::mat4x4&) const;
    void rotationProjection    () const;

    void stepAlongGaze         (bool);
    void verticalRotation      (int);
    void horizontalRotation    (int);
    Ray  getRay                (unsigned int,unsigned int) const;
    void updateProjection      (unsigned int,unsigned int
                               ,unsigned int,unsigned int);
    void updateProjection      ();

  private:
    class Impl;
    Impl* impl;
};

#endif

