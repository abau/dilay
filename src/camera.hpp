#ifndef DILAY_CAMERA
#define DILAY_CAMERA

#include "fwd-glm.hpp"

class CameraImpl;
class Ray;

class Camera {
  public:
     Camera            ();
     Camera            (const Camera&);
     Camera& operator= (const Camera&);
    ~Camera            ();

    void         initialize            ();

    unsigned int resolutionWidth       () const;
    unsigned int resolutionHeight      () const;

    void         updateResolution      (unsigned int, unsigned int);
    void         modelViewProjection   (const glm::mat4x4&) const;
    void         rotationProjection    () const;

    void         stepAlongGaze         (bool);
    void         verticalRotation      (int);
    void         horizontalRotation    (int);
    Ray          getRay                (unsigned int,unsigned int) const;
    void         updateProjection      (unsigned int,unsigned int
                                       ,unsigned int,unsigned int);
    void         updateProjection      ();

  private:
    CameraImpl* impl;
};

#endif

