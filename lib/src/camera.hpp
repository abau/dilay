#ifndef DILAY_CAMERA
#define DILAY_CAMERA

#include <glm/fwd.hpp>
#include "configurable.hpp"
#include "macro.hpp"

enum class Dimension;
class PrimRay;
class Renderer;

class Camera : public Configurable {
  public:
    DECLARE_BIG3 (Camera, const Config&)

          Renderer&    renderer        () const;
    const glm::uvec2&  resolution      () const;
    const glm::vec3&   gazePoint       () const;
    const glm::vec3&   toEyePoint      () const;
    const glm::vec3&   up              () const;
    const glm::vec3&   right           () const;
    const glm::mat4x4& view            () const;
    const glm::mat4x4& viewRotation    () const;
          glm::vec3    position        () const;
          glm::mat4x4  world           () const;

    void        updateResolution       (const glm::uvec2&);
    void        setModelViewProjection (const glm::mat4x4&, const glm::mat3x3&, bool);

    void        set                    (const glm::vec3&, const glm::vec3&, const glm::vec3&);
    void        setGaze                (const glm::vec3&);
    void        stepAlongGaze          (float);
    void        verticalRotation       (float);
    void        horizontalRotation     (float);
    glm::ivec2  fromWorld              (const glm::vec3&, const glm::mat4x4&, bool) const;
    glm::vec3   toWorld                (const glm::ivec2&, float = 0.0f) const;
    PrimRay     ray                    (const glm::ivec2&) const;
    Dimension   primaryDimension       () const;

  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif

