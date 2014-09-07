#ifndef DILAY_RENDERER
#define DILAY_RENDERER

#include <GL/glew.h>
// after glew:
#include <GL/gl.h>
#include <glm/fwd.hpp>
#include "macro.hpp"

class Color;
enum  class RenderMode;
class Camera;

class Renderer {
  public:
    static Renderer& global ();

    static void initialize         ();
    static void shutdown           ();
    static void renderInitialize   ();
    static void setProgram         (const RenderMode&);
    static void setMvp             (const GLfloat*);
    static void setModel           (const GLfloat*);
    static void setColor3          (const Color&);
    static void setColor4          (const Color&);
    static void setAmbient         (const Color&);
    static void setEyePoint        (const glm::vec3&);
    static void setLightPosition   (unsigned int, const glm::vec3&);
    static void setLightColor      (unsigned int, const Color&);
    static void setLightIrradiance (unsigned int, float);
    static void updateLights       (const Camera&);

  private:
    DECLARE_BIG3 (Renderer)

    IMPLEMENTATION
};

#endif
