#ifndef DILAY_RENDERER
#define DILAY_RENDERER

#include <GL/glew.h>
#include <GL/gl.h>

class RendererImpl;
class Color;
enum  class RenderMode;
class Camera;

class Renderer {
  public:
    static Renderer& global ();

    void initialize         ();
    void shutdown           ();
    void setProgram         (const RenderMode&);
    void setMvp             (const GLfloat*);
    void setColor3          (const Color&);
    void setColor4          (const Color&);
    void setAmbient         (const Color&);
    void setLightPosition   (unsigned int, const glm::vec3&);
    void setLightColor      (unsigned int, const Color&);
    void setLightIrradiance (unsigned int, float);
    void updateLights       (const Camera&);

  private:
          Renderer            ();
          Renderer            (const Renderer&) = delete;
    const Renderer& operator= (const Renderer&) = delete;
         ~Renderer            ();

    RendererImpl* impl;
};

#endif
