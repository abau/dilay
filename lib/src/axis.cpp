#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <QPainter>
#include "axis.hpp"
#include "renderer.hpp"
#include "mesh.hpp"
#include "state.hpp"
#include "macro.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "render-mode.hpp"
#include "config.hpp"

struct Axis::Impl {
  Mesh       coneMesh;
  Mesh       cylinderMesh;
  glm::uvec2 axisResolution;
  Color      axisLabelColor;

  void initialize () {
    this->axisResolution = glm::uvec2 (200,200);

    this->axisLabelColor = Config::get <Color> ("/editor/color/axis-label");
    Color axisColor      = Config::get <Color> ("/editor/color/axis");

    this->coneMesh       = Mesh::cone     (10);
    this->cylinderMesh   = Mesh::cylinder (10);

    this->cylinderMesh.scaling    (glm::vec3 (0.01f, 0.3f , 0.01f));
    this->cylinderMesh.renderMode (RenderMode::Color);
    this->cylinderMesh.color      (axisColor);
    this->cylinderMesh.bufferData ();

    this->coneMesh.scaling        (glm::vec3 (0.03f, 0.1f, 0.03f));
    this->coneMesh.renderMode     (RenderMode::Color);
    this->coneMesh.color          (axisColor);
    this->coneMesh.bufferData     ();
  }

  void render () {
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::uvec2 resolution = State::camera ().resolution ();
    State::camera ().updateResolution (glm::uvec2(200,200));

    this->cylinderMesh.position       (glm::vec3 (0.0f, 0.15f, 0.0f));
    this->cylinderMesh.rotationMatrix (glm::mat4x4 (1.0f));
    this->cylinderMesh.render         (true);

    this->cylinderMesh.position       (glm::vec3 (0.15f, 0.0f, 0.0f));
    this->cylinderMesh.rotationZ      (0.5f * glm::pi<float> ());
    this->cylinderMesh.render         (true);

    this->cylinderMesh.position       (glm::vec3 (0.0f, 0.0f, 0.15f));
    this->cylinderMesh.rotationX      (0.5f * glm::pi<float> ());
    this->cylinderMesh.render         (true);

    this->coneMesh.position           (glm::vec3 (0.0f, 0.3f, 0.0f));
    this->coneMesh.rotationMatrix     (glm::mat4x4 (1.0f));
    this->coneMesh.render             (true);

    this->coneMesh.position           (glm::vec3 (0.3f, 0.0f, 0.0f));
    this->coneMesh.rotationZ          (- 0.5f * glm::pi<float> ());
    this->coneMesh.render             (true);

    this->coneMesh.position           (glm::vec3 (0.0f, 0.0f, 0.3f));
    this->coneMesh.rotationX          (0.5f * glm::pi<float> ());
    this->coneMesh.render             (true);

    State :: camera ().updateResolution (resolution);

    this->coneMesh.renderEnd ();
  }

  void render (QPainter& painter) {
    this->coneMesh.rotationMatrix (glm::mat4x4 (1.0f));

    glm::uvec2 resolution = State::camera ().resolution ();
    State::camera ().updateResolution (this->axisResolution);

    auto f = [this,&resolution,&painter] (const glm::vec3& p, const QString& l) {
      this->coneMesh.position (p);

      glm::ivec2 pos = State::camera ().fromWorld ( glm::vec3 (0.0f)
                                                  , this->coneMesh.modelMatrix ()
                                                  , true);

      painter.drawText (pos.x, resolution.y - this->axisResolution.y + pos.y, l);
    };

    painter.setPen  (this->axisLabelColor.qColor ());
    painter.setFont (this->makeFont ());

    f (glm::vec3 (0.3f, 0.0f, 0.0f), "X");
    f (glm::vec3 (0.0f, 0.3f, 0.0f), "Y");
    f (glm::vec3 (0.0f, 0.0f, 0.3f), "Z");

    State :: camera ().updateResolution (resolution);
  }

  QFont makeFont () {
    QFont font;
    font.setWeight (QFont::Bold);
    return font;
  }
};

DELEGATE_BIG3 (Axis)
DELEGATE      (void, Axis, initialize)
DELEGATE      (void, Axis, render)
DELEGATE1     (void, Axis, render, QPainter&)
