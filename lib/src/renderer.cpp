#include <glm/glm.hpp>
#include "color.hpp"
#include "config.hpp"
#include "macro.hpp"
#include "opengl.hpp"
#include "render-mode.hpp"
#include "renderer.hpp"

namespace {
  const unsigned int numLights = 2;

  struct LightIds {
    int directionId;
    int colorId;
    int irradianceId;

    LightIds () {
      this->directionId  = 0;
      this->colorId      = 0;
      this->irradianceId = 0;
    }
  };

  struct ShaderIds {
    unsigned int programId;
    int          mvpId;
    int          modelId;
    int          colorId;
    int          wireframeColorId;
    int          ambientId;
    int          eyePointId;
    int          barycentricId;
    LightIds     lightIds [numLights];

    ShaderIds () {
      this->programId        = 0;
      this->mvpId            = 0;
      this->modelId          = 0;
      this->colorId          = 0;
      this->wireframeColorId = 0;
      this->ambientId        = 0;
      this->eyePointId       = 0;
      this->barycentricId    = 0;
    }
  };

  struct GlobalLightUniforms {
    glm::vec3 direction;
    glm::vec3 transformedDirection;
    Color     color;
    float     irradiance;
  };

  struct GlobalUniforms {
    GlobalLightUniforms lightUniforms [numLights];
    Color               ambient;
    glm::vec3           eyePoint;
  };
};

struct Renderer::Impl {
  static const unsigned int numShaders = 6;

  ShaderIds      shaderIds [Impl::numShaders];
  ShaderIds*     activeShaderIndex;
  GlobalUniforms globalUniforms;
  Color          clearColor;

  Impl (const Config& config) 
    : activeShaderIndex (nullptr) 
  {
    this->runFromConfig (config);
  }

  ~Impl () {
    for (unsigned int i = 0; i < Impl::numShaders; i++) {
      OpenGL::safeDeleteProgram (this->shaderIds[i].programId);
    }
  }

  void setupRendering () {
    OpenGL::glClearColor ( this->clearColor.r ()
                         , this->clearColor.g ()
                         , this->clearColor.b (), 0.0f);
        
    OpenGL::glFrontFace (OpenGL::CCW       ());
    OpenGL::glEnable    (OpenGL::CullFace  ());
    OpenGL::glCullFace  (OpenGL::Back      ());
    OpenGL::glEnable    (OpenGL::DepthTest ()); 
    OpenGL::glDepthFunc (OpenGL::LEqual    ()); 
    OpenGL::glClear     ( OpenGL::ColorBufferBit ()
                        | OpenGL::DepthBufferBit () );
  }

  unsigned int shaderIndex (const RenderMode& renderMode) {
    if (renderMode.smoothShading ()) {
      return renderMode.renderWireframe () ? 0 : 1;
    }
    else if (renderMode.flatShading ()) {
      return renderMode.renderWireframe () ? 2 : 3;
    }
    else if (renderMode.constantShading ()) {
      return renderMode.renderWireframe () ? 4 : 5;
    }
    else {
      std::abort ();
    }
  }

  void initalizeProgram (const RenderMode& renderMode) {
    assert ( renderMode.renderWireframe () == false
          || OpenGL::supportsGeometryShader () );

    const unsigned int id = OpenGL::loadProgram ( renderMode.vertexShader ()
                                                , renderMode.fragmentShader ()
                                                , renderMode.renderWireframe () );

    unsigned int index = this->shaderIndex (renderMode);
    assert (this->shaderIds[index].programId == 0);

    ShaderIds *s = &this->shaderIds [index];

    s->programId                = id;
    s->mvpId                    = OpenGL::glGetUniformLocation (id, "mvp");
    s->modelId                  = OpenGL::glGetUniformLocation (id, "model");
    s->colorId                  = OpenGL::glGetUniformLocation (id, "color");
    s->wireframeColorId         = OpenGL::glGetUniformLocation (id, "wireframeColor");
    s->ambientId                = OpenGL::glGetUniformLocation (id, "ambient");
    s->eyePointId               = OpenGL::glGetUniformLocation (id, "eyePoint");
    s->barycentricId            = OpenGL::glGetUniformLocation (id, "barycentric");
    s->lightIds[0].directionId  = OpenGL::glGetUniformLocation (id, "light1Direction");
    s->lightIds[0].colorId      = OpenGL::glGetUniformLocation (id, "light1Color");
    s->lightIds[0].irradianceId = OpenGL::glGetUniformLocation (id, "light1Irradiance");
    s->lightIds[1].directionId  = OpenGL::glGetUniformLocation (id, "light2Direction");
    s->lightIds[1].colorId      = OpenGL::glGetUniformLocation (id, "light2Color");
    s->lightIds[1].irradianceId = OpenGL::glGetUniformLocation (id, "light2Irradiance");
  }

  void setProgram (const RenderMode& renderMode) {
    const unsigned int index = this->shaderIndex (renderMode);

    if (this->shaderIds[index].programId == 0) {
      this->initalizeProgram (renderMode);
    }
    assert (this->shaderIds[index].programId);

    this->activeShaderIndex = &this->shaderIds[index];
    OpenGL::glUseProgram (this->activeShaderIndex->programId);

    OpenGL::glUniformVec3 
      (this->activeShaderIndex->ambientId, this->globalUniforms.ambient.vec3 ());

    OpenGL::glUniformVec3 
      (this->activeShaderIndex->eyePointId, this->globalUniforms.eyePoint);

    for (unsigned int i = 0; i < numLights; i++) {
      OpenGL::glUniformVec3 
        ( this->activeShaderIndex->lightIds[i].directionId
        , this->globalUniforms.lightUniforms[i].transformedDirection);
      OpenGL::glUniformVec3 
        ( this->activeShaderIndex->lightIds[i].colorId
        , this->globalUniforms.lightUniforms[i].color.vec3 ());
      OpenGL::glUniform1f ( this->activeShaderIndex->lightIds[i].irradianceId
                          , this->globalUniforms.lightUniforms[i].irradiance );
    }
  }

  void setMvp (const float* mvp) {
    assert (this->activeShaderIndex);
    OpenGL::glUniformMatrix4fv (this->activeShaderIndex->mvpId, 1, false, mvp);
  }

  void setModel (const float* model) {
    assert (this->activeShaderIndex);
    OpenGL::glUniformMatrix4fv (this->activeShaderIndex->modelId, 1, false, model);
  }

  void setColor3 (const Color& c) {
    assert (this->activeShaderIndex);
    OpenGL::glUniformVec3 (this->activeShaderIndex->colorId, c.vec3 ());
  }

  void setColor4 (const Color& c) {
    assert (this->activeShaderIndex);
    OpenGL::glUniformVec4 (this->activeShaderIndex->colorId, c.vec4 ());
  }

  void setWireframeColor3 (const Color& c) {
    assert (this->activeShaderIndex);
    OpenGL::glUniformVec3 (this->activeShaderIndex->wireframeColorId, c.vec3 ());
  }

  void setWireframeColor4 (const Color& c) {
    assert (this->activeShaderIndex);
    OpenGL::glUniformVec4 (this->activeShaderIndex->wireframeColorId, c.vec4 ());
  }

  void setAmbient (const Color& c) {
    this->globalUniforms.ambient = c;
  }

  void setEyePoint (const glm::vec3& e) {
    this->globalUniforms.eyePoint = e;
  }

  void setLightDirection (unsigned int i, const glm::vec3& d) {
    assert (i < numLights);
    this->globalUniforms.lightUniforms[i].direction = d;
  }

  void setLightColor (unsigned int i, const Color& c) {
    assert (i < numLights);
    this->globalUniforms.lightUniforms[i].color = c;
  }

  void setLightIrradiance (unsigned int i, float irr) {
    assert (i < numLights);
    this->globalUniforms.lightUniforms[i].irradiance = irr;
  }

  void updateLights (const glm::mat4x4& world) {
    const glm::mat3x3 w (world);

    this->globalUniforms.lightUniforms[0].transformedDirection = 
      w * this->globalUniforms.lightUniforms[0].direction;

    this->globalUniforms.lightUniforms[1].transformedDirection = 
      w * this->globalUniforms.lightUniforms[1].direction;
  }

  void runFromConfig (const Config& config) {
    this->clearColor     = config.get<Color>     ("editor/background");
    const glm::vec3 dir1 = config.get<glm::vec3> ("editor/light/light1/direction");
    const glm::vec3 dir2 = config.get<glm::vec3> ("editor/light/light2/direction");

    this->setAmbient         (   config.get<Color>     ("editor/light/ambient"));
    this->setLightDirection  (0, glm::normalize (dir1));
    this->setLightColor      (0, config.get<Color>     ("editor/light/light1/color"));
    this->setLightIrradiance (0, config.get<float>     ("editor/light/light1/irradiance"));
    this->setLightDirection  (1, glm::normalize (dir2));
    this->setLightColor      (1, config.get<Color>     ("editor/light/light2/color"));
    this->setLightIrradiance (1, config.get<float>     ("editor/light/light2/irradiance"));
  }
};

DELEGATE1_BIG3 (Renderer, const Config&)

DELEGATE  (void, Renderer, setupRendering)
DELEGATE1 (void, Renderer, setProgram        , const RenderMode&)
DELEGATE1 (void, Renderer, setMvp            , const float*)
DELEGATE1 (void, Renderer, setModel          , const float*)
DELEGATE1 (void, Renderer, setColor3         , const Color&)
DELEGATE1 (void, Renderer, setColor4         , const Color&)
DELEGATE1 (void, Renderer, setWireframeColor3, const Color&)
DELEGATE1 (void, Renderer, setWireframeColor4, const Color&)
DELEGATE1 (void, Renderer, setAmbient        , const Color&)
DELEGATE1 (void, Renderer, setEyePoint       , const glm::vec3&)
DELEGATE2 (void, Renderer, setLightDirection , unsigned int, const glm::vec3&)
DELEGATE2 (void, Renderer, setLightColor     , unsigned int, const Color&)
DELEGATE2 (void, Renderer, setLightIrradiance, unsigned int, float)
DELEGATE1 (void, Renderer, updateLights      , const glm::mat4x4&)
DELEGATE1 (void, Renderer, runFromConfig     , const Config&)
