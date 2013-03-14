#ifndef DILAY_RENDERER
#define DILAY_RENDERER

#include <GL/glew.h>
#include <GL/gl.h>

class RendererImpl;
class Color;
class ViewLight;
enum  RenderMode : short;

class Renderer {
  public:
    static Renderer& global ();

    void initialize    ();
    void shutdown      ();
    void setColor3     (const Color&);
    void setColor4     (const Color&);
    void setMvp        (const GLfloat*);
    void setProgram    (const RenderMode&);
    void setViewLight1 (const ViewLight&);

  private:
     Renderer            ();
     Renderer            (const Renderer&) = delete;
     Renderer& operator= (const Renderer&) = delete;
    ~Renderer            ();

    RendererImpl* impl;
};

#endif
