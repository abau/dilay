#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <QPainter>
#include "camera.hpp"
#include "color.hpp"
#include "config.hpp"
#include "dimension.hpp"
#include "macro.hpp"
#include "mesh-definition.hpp"
#include "mesh.hpp"
#include "opengl.hpp"
#include "render-flags.hpp"
#include "render-mode.hpp"
#include "state.hpp"
#include "view/axis.hpp"

struct ViewAxis::Impl {
  Mesh         coneMesh;
  Mesh         cylinderMesh;
  Mesh         gridMesh;
  glm::uvec2   axisResolution;
  Color        axisColor;
  Color        axisLabelColor;
  unsigned int gridResolution;

  Impl (const Config& config) {
    this->runFromConfig (config);

    this->axisResolution = glm::uvec2 (200,200);
    this->gridResolution = 6;

    this->coneMesh       = Mesh (MeshDefinition::Cone     (10));
    this->cylinderMesh   = Mesh (MeshDefinition::Cylinder (10));

    this->cylinderMesh.scaling    (glm::vec3 (0.01f, 0.3f , 0.01f));
    this->cylinderMesh.renderMode (RenderMode::Constant);
    this->cylinderMesh.color      (this->axisColor);
    this->cylinderMesh.bufferData ();

    this->coneMesh.scaling        (glm::vec3 (0.03f, 0.1f, 0.03f));
    this->coneMesh.renderMode     (RenderMode::Constant);
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
    this->gridMesh.renderMode (RenderMode::Constant);
    this->gridMesh.bufferData ();
    this->gridMesh.color      (this->axisColor);
  }

  void render (Camera& camera) {
    OpenGL::glClear (OpenGL::DepthBufferBit ());

    const glm::uvec2  resolution = camera.resolution ();
    const RenderFlags flags      = RenderFlags::NoZoom ();
    camera.updateResolution (glm::uvec2 (200,200));

    this->cylinderMesh.position       (glm::vec3 (0.0f, 0.15f, 0.0f));
    this->cylinderMesh.rotationMatrix (glm::mat4x4 (1.0f));
    this->cylinderMesh.render         (camera, flags);

    this->cylinderMesh.position       (glm::vec3 (0.15f, 0.0f, 0.0f));
    this->cylinderMesh.rotationZ      (0.5f * glm::pi<float> ());
    this->cylinderMesh.render         (camera, flags);

    this->cylinderMesh.position       (glm::vec3 (0.0f, 0.0f, 0.15f));
    this->cylinderMesh.rotationX      (0.5f * glm::pi<float> ());
    this->cylinderMesh.render         (camera, flags);

    this->coneMesh.position           (glm::vec3 (0.0f, 0.3f, 0.0f));
    this->coneMesh.rotationMatrix     (glm::mat4x4 (1.0f));
    this->coneMesh.render             (camera, flags);

    this->coneMesh.position           (glm::vec3 (0.3f, 0.0f, 0.0f));
    this->coneMesh.rotationZ          (- 0.5f * glm::pi<float> ());
    this->coneMesh.render             (camera, flags);

    this->coneMesh.position           (glm::vec3 (0.0f, 0.0f, 0.3f));
    this->coneMesh.rotationX          (0.5f * glm::pi<float> ());
    this->coneMesh.render             (camera, flags);

    this->renderGrid                  (camera, flags);

    camera.updateResolution (resolution);
  }

  void renderGrid (Camera& camera, const RenderFlags& flags) {
    switch (camera.primaryDimension ()) {
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
    this->gridMesh.renderLines (camera, flags);
  }

  void render (Camera& camera, QPainter& painter) {
    this->coneMesh.rotationMatrix (glm::mat4x4 (1.0f));

    QFont        font       = this->makeFont ();
    QFontMetrics metrics (font); 
    int          w          = glm::max (metrics.maxWidth (), metrics.height ());
    glm::uvec2   resolution = camera.resolution ();
    camera.updateResolution (this->axisResolution);

    auto f = [this, &resolution, &painter, w, &camera] 
             (const glm::vec3& p, const QString& l) 
    {
      this->coneMesh.position (p);

      glm::ivec2 pos = camera.fromWorld ( glm::vec3 (0.0f)
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

    camera.updateResolution (resolution);
  }

  QFont makeFont () {
    QFont font;
    font.setWeight (QFont::Bold);
    return font;
  }

  void runFromConfig (const Config& config) {
    this->axisLabelColor = config.get <Color> ("editor/axis/color/label");
    this->axisColor      = config.get <Color> ("editor/axis/color/normal");
  }
};

DELEGATE1_BIG3 (ViewAxis, const Config&)
DELEGATE1 (void, ViewAxis, render, Camera&)
DELEGATE2 (void, ViewAxis, render, Camera&, QPainter&)
DELEGATE1 (void, ViewAxis, runFromConfig, const Config&)
