#include <glm/glm.hpp>
#include "color.hpp"
#include "config.hpp"
#include "macro.hpp"
#include "opengl.hpp"
#include "render-mode.hpp"
#include "renderer.hpp"
#include "shader.hpp"

namespace {
  const unsigned int numLights = 2;

  struct LightIds {
    int positionId;
    int colorId;
    int irradianceId;

    LightIds () {
      this->positionId   = 0;
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
    glm::vec3 position;
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
  ShaderIds      shaderIds [RenderModeUtil :: numRenderModes];
  ShaderIds*     activeShaderIndex;
  GlobalUniforms globalUniforms;
  Color          clearColor;
  glm::vec3      light1LocalPos;
  glm::vec3      light2LocalPos;

  Impl (const Config& config) : activeShaderIndex (nullptr) {

    auto getUniformLocations = [this] (RenderMode mode) {
      ShaderIds    *s             = &this->shaderIds [static_cast <int> (mode)];
      unsigned int p              = s->programId;
      s->mvpId                    = OpenGL::glGetUniformLocation (p, "mvp");
      s->modelId                  = OpenGL::glGetUniformLocation (p, "model");
      s->colorId                  = OpenGL::glGetUniformLocation (p, "color");
      s->wireframeColorId         = OpenGL::glGetUniformLocation (p, "wireframeColor");
      s->ambientId                = OpenGL::glGetUniformLocation (p, "ambient");
      s->eyePointId               = OpenGL::glGetUniformLocation (p, "eyePoint");
      s->barycentricId            = OpenGL::glGetUniformLocation (p, "barycentric");
      s->lightIds[0].positionId   = OpenGL::glGetUniformLocation (p, "light1Position");
      s->lightIds[0].colorId      = OpenGL::glGetUniformLocation (p, "light1Color");
      s->lightIds[0].irradianceId = OpenGL::glGetUniformLocation (p, "light1Irradiance");
      s->lightIds[1].positionId   = OpenGL::glGetUniformLocation (p, "light2Position");
      s->lightIds[1].colorId      = OpenGL::glGetUniformLocation (p, "light2Color");
      s->lightIds[1].irradianceId = OpenGL::glGetUniformLocation (p, "light2Irradiance");
    };

    this->runFromConfig (config);

    this->shaderIds [static_cast <int> (RenderMode::Smooth)].programId =
      OpenGL::loadProgram ( Shader::smoothVertexShader   ()
                          , Shader::smoothFragmentShader ()
                          , requiresGeometryShader (RenderMode::Smooth)
                          );
    getUniformLocations (RenderMode::Smooth);

    this->shaderIds [static_cast <int> (RenderMode::Flat)].programId =
      OpenGL::loadProgram ( Shader::flatVertexShader   ()
                          , Shader::flatFragmentShader ()
                          , requiresGeometryShader (RenderMode::Flat)
                          );
    getUniformLocations (RenderMode::Flat);

    this->shaderIds [static_cast <int> (RenderMode::Constant)].programId =
      OpenGL::loadProgram ( Shader::constantVertexShader   ()
                          , Shader::constantFragmentShader ()
                          , requiresGeometryShader (RenderMode::Constant)
                          );
    getUniformLocations (RenderMode::Constant);

    if (OpenGL::supportsGeometryShader ()) {
      this->shaderIds [static_cast <int> (RenderMode::SmoothWireframe)].programId =
        OpenGL::loadProgram ( Shader::smoothVertexShader            ()
                            , Shader::smoothWireframeFragmentShader ()
                            , requiresGeometryShader (RenderMode::SmoothWireframe)
                            );
      getUniformLocations (RenderMode::SmoothWireframe);

      this->shaderIds [static_cast <int> (RenderMode::FlatWireframe)].programId =
        OpenGL::loadProgram ( Shader::flatVertexShader            ()
                            , Shader::flatWireframeFragmentShader ()
                            , requiresGeometryShader (RenderMode::FlatWireframe)
                            );
      getUniformLocations (RenderMode::FlatWireframe);

      this->shaderIds [static_cast <int> (RenderMode::ConstantWireframe)].programId =
        OpenGL::loadProgram ( Shader::constantVertexShader            ()
                            , Shader::constantWireframeFragmentShader ()
                            , requiresGeometryShader (RenderMode::ConstantWireframe)
                            );
      getUniformLocations (RenderMode::ConstantWireframe);
    }
  }

  ~Impl () {
    for (unsigned int rm = 0; rm < RenderModeUtil :: numRenderModes; rm++) {
      OpenGL::safeDeleteProgram (this->shaderIds[rm].programId);
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

  void setProgram (RenderMode renderMode) {
    const int shaderIndex = static_cast <int> (renderMode);

    assert (this->shaderIds[shaderIndex].programId);

    this->activeShaderIndex = &this->shaderIds[shaderIndex];
    OpenGL::glUseProgram (this->activeShaderIndex->programId);

    OpenGL::glUniformVec3 
      (this->activeShaderIndex->ambientId, this->globalUniforms.ambient.vec3 ());

    OpenGL::glUniformVec3 
      (this->activeShaderIndex->eyePointId, this->globalUniforms.eyePoint);

    for (unsigned int i = 0; i < numLights; i++) {
      OpenGL::glUniformVec3 
        ( this->activeShaderIndex->lightIds[i].positionId
        , this->globalUniforms.lightUniforms[i].position);
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

  void setLightPosition (unsigned int i, const glm::vec3& p) {
    assert (i < numLights);
    this->globalUniforms.lightUniforms[i].position = glm::normalize (p);
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

    this->setLightPosition (0, w * this->light1LocalPos);
    this->setLightPosition (1, w * this->light2LocalPos);
  }

  static bool requiresGeometryShader (RenderMode mode) {
    return mode == RenderMode::SmoothWireframe
        || mode == RenderMode::FlatWireframe
        || mode == RenderMode::ConstantWireframe;
  }

  static bool requiresNormalAttribute (RenderMode mode) {
    return mode == RenderMode::Smooth
        || mode == RenderMode::SmoothWireframe;
  }

  void runFromConfig (const Config& config) {
    this->clearColor     = config.get<Color>     ("editor/background");
    this->light1LocalPos = config.get<glm::vec3> ("editor/light/light1/position");
    this->light2LocalPos = config.get<glm::vec3> ("editor/light/light2/position");

    this->setAmbient         (   config.get<Color>     ("editor/light/ambient"));
    this->setLightPosition   (0, this->light1LocalPos);
    this->setLightColor      (0, config.get<Color>     ("editor/light/light1/color"));
    this->setLightIrradiance (0, config.get<float>     ("editor/light/light1/irradiance"));
    this->setLightPosition   (1, this->light2LocalPos);
    this->setLightColor      (1, config.get<Color>     ("editor/light/light2/color"));
    this->setLightIrradiance (1, config.get<float>     ("editor/light/light2/irradiance"));
  }
};

DELEGATE1_BIG3 (Renderer, const Config&)

DELEGATE  (void, Renderer, setupRendering)
DELEGATE1 (void, Renderer, setProgram        , RenderMode)
DELEGATE1 (void, Renderer, setMvp            , const float*)
DELEGATE1 (void, Renderer, setModel          , const float*)
DELEGATE1 (void, Renderer, setColor3         , const Color&)
DELEGATE1 (void, Renderer, setColor4         , const Color&)
DELEGATE1 (void, Renderer, setWireframeColor3, const Color&)
DELEGATE1 (void, Renderer, setWireframeColor4, const Color&)
DELEGATE1 (void, Renderer, setAmbient        , const Color&)
DELEGATE1 (void, Renderer, setEyePoint       , const glm::vec3&)
DELEGATE2 (void, Renderer, setLightPosition  , unsigned int, const glm::vec3&)
DELEGATE2 (void, Renderer, setLightColor     , unsigned int, const Color&)
DELEGATE2 (void, Renderer, setLightIrradiance, unsigned int, float)
DELEGATE1 (void, Renderer, updateLights      , const glm::mat4x4&)
DELEGATE1_STATIC (bool, Renderer, requiresGeometryShader, RenderMode)
DELEGATE1_STATIC (bool, Renderer, requiresNormalAttribute, RenderMode)
DELEGATE1 (void, Renderer, runFromConfig     , const Config&)
