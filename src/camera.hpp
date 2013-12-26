#ifndef DILAY_CAMERA
#define DILAY_CAMERA

#include "macro.hpp"
#include "fwd-glm.hpp"

class Ray;

class Camera {
  public:
    DECLARE_BIG5 (Camera)

    void               initialize            ();

    const glm::uvec2&  resolution            () const;
    const glm::vec3&   gazePoint             () const;
    const glm::vec3&   toEyePoint            () const;
    const glm::vec3&   up                    () const;
    const glm::vec3&   right                 () const;
    const glm::mat4x4& view                  () const;
    const glm::mat4x4& viewRotation          () const;
          glm::vec3    position              () const;
          glm::mat4x4  world                 () const;

    void      updateResolution    (const glm::uvec2&);
    void      modelViewProjection (const glm::mat4x4&) const;
    void      rotationProjection  () const;

    void      stepAlongGaze       (bool);
    void      verticalRotation    (float);
    void      horizontalRotation  (float);
    glm::vec3 toWorld             (const glm::uvec2&, float = 0.0f) const;
    Ray       getRay              (const glm::uvec2&) const;
    void      updateProjection    (const glm::uvec2&, const glm::uvec2&);
    void      updateProjection    ();

  private:
    class Impl;
    Impl* impl;
};

#endif

