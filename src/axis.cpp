#include <glm/glm.hpp>
#include "axis.hpp"
#include "renderer.hpp"
#include "mesh.hpp"
#include "state.hpp"
#include "macro.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "render-mode.hpp"

struct Axis::Impl {
  Mesh   mesh;
  float  length;

  Impl () : length (0.3f) {}

  void initialize () {
    this->mesh.addVertex (glm::vec3 (0.0f        , 0.0f        , 0.0f        ));
    this->mesh.addVertex (glm::vec3 (this->length, 0.0f        , 0.0f        ));
    this->mesh.addVertex (glm::vec3 (0.0f        , this->length, 0.0f        ));
    this->mesh.addVertex (glm::vec3 (0.0f        , 0.0f        , this->length));

    this->mesh.addIndex (0);
    this->mesh.addIndex (1);
    this->mesh.addIndex (0);
    this->mesh.addIndex (2);
    this->mesh.addIndex (0);
    this->mesh.addIndex (3);

    this->mesh.renderMode (RenderMode::Wireframe);
    this->mesh.bufferData ();
  }

  void render () {
    this->mesh.renderBegin ();

    State :: camera ().updateProjection (glm::uvec2(0),glm::uvec2(200,200));
    State :: camera ().rotationProjection ();

    glDisable (GL_DEPTH_TEST);

    Renderer :: setColor3 (Color (1.0f,0.0f,0.0f));
    glDrawElements (GL_LINES, 2, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * 0));

    Renderer :: setColor3 (Color (0.0f,1.0f,0.0f));
    glDrawElements (GL_LINES, 2, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * 2));

    Renderer :: setColor3 (Color (0.0f,0.0f,1.0f));
    glDrawElements (GL_LINES, 2, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * 4));

    glEnable (GL_DEPTH_TEST);

    State :: camera ().updateProjection ();

    this->mesh.renderEnd ();
  }
};

DELEGATE_CONSTRUCTOR (Axis)
DELEGATE_DESTRUCTOR  (Axis)
DELEGATE             (void, Axis, initialize)
DELEGATE             (void, Axis, render)
