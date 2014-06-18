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
#include "dimension.hpp"

struct Axis::Impl {
  Mesh         coneMesh;
  Mesh         cylinderMesh;
  Mesh         gridMesh;
  glm::uvec2   axisResolution;
  Color        axisColor;
  Color        axisLabelColor;
  unsigned int gridResolution;

  void initialize () {
    this->axisResolution = glm::uvec2 (200,200);
    this->gridResolution = 6;

    this->axisLabelColor = Config::get <Color> ("/config/editor/axis/color/label");
    this->axisColor      = Config::get <Color> ("/config/editor/axis/color/normal");

    this->coneMesh       = Mesh::cone     (10);
    this->cylinderMesh   = Mesh::cylinder (10);

    this->cylinderMesh.scaling    (glm::vec3 (0.01f, 0.3f , 0.01f));
    this->cylinderMesh.renderMode (RenderMode::Color);
    this->cylinderMesh.color      (this->axisColor);
    this->cylinderMesh.bufferData ();

    this->coneMesh.scaling        (glm::vec3 (0.03f, 0.1f, 0.03f));
    this->coneMesh.renderMode     (RenderMode::Color);
    this->coneMesh.color          (this->axisColor);
    this->coneMesh.bufferData     ();

    this->initializeGrid          ();
  }

  void initializeGrid () {
    float gridSpace = 0.3f / float (gridResolution - 1);

    for (unsigned int j = 0; j < gridResolution; j++) {
      for (unsigned int i = 0; i < gridResolution; i++) {
        this->gridMesh.addVertex (glm::vec3 ( float (i) * gridSpace
                                            , float (j) * gridSpace
                                            , 0.0f));
      }
    }
    for (unsigned int j = 1; j < gridResolution; j++) {
      for (unsigned int i = 1; i < gridResolution; i++) {
        this->gridMesh.addIndex ((j*gridResolution)+i-1);
        this->gridMesh.addIndex ((j*gridResolution)+i);
        this->gridMesh.addIndex (((j-1)*gridResolution)+i);
        this->gridMesh.addIndex ((j*gridResolution)+i);
      }
    }
    this->gridMesh.renderMode (RenderMode::Color);
    this->gridMesh.bufferData ();
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

    this->renderGrid                  ();

    State::camera ().updateResolution (resolution);
  }

  void renderGrid () {
    unsigned int numLines = (this->gridResolution - 1) * (this->gridResolution - 1) * 4;

    switch (State::camera ().primaryDimension ()) {
      case Dimension::X:
        this->gridMesh.rotationY (- 0.5f * glm::pi<float> ());
        break;
      case Dimension::Y:
        this->gridMesh.rotationX (0.5f * glm::pi<float> ());
        break;
      case Dimension::Z:
        this->gridMesh.rotationMatrix (glm::mat4x4 (1.0f));
        break;
    }

    this->gridMesh.renderBegin (true);

    Renderer::setColor3 (this->axisColor);
    glDrawElements (GL_LINES, numLines, GL_UNSIGNED_INT, (GLvoid*)0);

    this->gridMesh.renderEnd ();
  }

  void render (QPainter& painter) {
    this->coneMesh.rotationMatrix (glm::mat4x4 (1.0f));

    QFont        font       = this->makeFont ();
    QFontMetrics metrics (font); 
    int          w          = glm::max (metrics.maxWidth (), metrics.height ());
    glm::uvec2   resolution = State::camera ().resolution ();
    State::camera ().updateResolution (this->axisResolution);

    auto f = [this,&resolution,&painter,w] (const glm::vec3& p, const QString& l) {
      this->coneMesh.position (p);

      glm::ivec2 pos = State::camera ().fromWorld ( glm::vec3 (0.0f)
                                                  , this->coneMesh.modelMatrix ()
                                                  , true);
      QRect rect ( pos.x - (w / 2)
                 , resolution.y - this->axisResolution.y + pos.y - (w / 2)
                 , w, w );

      painter.drawText (rect, Qt::AlignCenter, l);
    };

    painter.setPen  (this->axisLabelColor.qColor ());
    painter.setFont (font);

    f (glm::vec3 (0.35f, 0.0f , 0.0f ), "X");
    f (glm::vec3 (0.0f , 0.35f, 0.0f ), "Y");
    f (glm::vec3 (0.0f , 0.0f , 0.35f), "Z");

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
