#ifndef DILAY_RENDERER
#define DILAY_RENDERER

#include <GL/glew.h>
#include <GL/gl.h>

class Color;
enum  class RenderMode;
class Camera;

class Renderer {
  public:
    static Renderer& global ();

    static void initialize         ();
    static void shutdown           ();
    static void setProgram         (const RenderMode&);
    static void setMvp             (const GLfloat*);
    static void setColor3          (const Color&);
    static void setColor4          (const Color&);
    static void setAmbient         (const Color&);
    static void setLightPosition   (unsigned int, const glm::vec3&);
    static void setLightColor      (unsigned int, const Color&);
    static void setLightIrradiance (unsigned int, float);
    static void updateLights       (const Camera&);

  private:
          Renderer            ();
          Renderer            (const Renderer&) = delete;
    const Renderer& operator= (const Renderer&) = delete;
         ~Renderer            ();

    class Impl;
    Impl* impl;
};

#endif
