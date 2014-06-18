#include <stdexcept>
#include <glm/glm.hpp>
#include "renderer.hpp"
#include "config.hpp"
#include "opengl-util.hpp"
#include "render-mode.hpp"
#include "color.hpp"
#include "macro.hpp"
#include "camera.hpp"
#include "shader.hpp"

const unsigned int numLights = 2;

struct LightIds {
  GLuint positionId;
  GLuint colorId;
  GLuint irradianceId;

  LightIds () {
    this->positionId   = 0;
    this->colorId      = 0;
    this->irradianceId = 0;
  }
};

struct ShaderIds {
  GLuint   programId;
  GLuint   mvpId;
  GLuint   modelId;
  GLuint   colorId;
  GLuint   ambientId;
  GLuint   eyePointId;
  LightIds lightIds [numLights];

  ShaderIds () {
    this->programId  = 0;
    this->mvpId      = 0;
    this->modelId    = 0;
    this->colorId    = 0;
    this->ambientId  = 0;
    this->eyePointId = 0;
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

struct Renderer::Impl {
  ShaderIds      shaderIds [RenderModeUtil :: numRenderModes];
  ShaderIds*     activeShaderIndex;
  GlobalUniforms globalUniforms;
  Color          clearColor;

  Impl () : activeShaderIndex (nullptr) {}

  void initialize () {
    static bool isInitialized = false;

    if ( ! isInitialized ) {
      GLenum err = glewInit ();
      if (err  != GLEW_OK ) {
        std::string e1 = std::string ("Error while initializing glew: ");
        std::string e2 = std::string ((const char*)glewGetErrorString (err));
        throw (std::runtime_error (e1 + e2));
      }

      this->clearColor = Config::get<Color> ("/config/editor/color/background");

      this->shaderIds [static_cast <int> (RenderMode::SmoothShaded)].programId =
        OpenGLUtil :: loadProgram ( Shader::smoothVertexShader   ()
                                  , Shader::smoothFragmentShader ()
                                  );
      this->shaderIds [static_cast <int> (RenderMode::Wireframe)].programId =
        OpenGLUtil :: loadProgram ( Shader::simpleVertexShader   ()
                                  , Shader::simpleFragmentShader ()
                                  );
      this->shaderIds [static_cast <int> (RenderMode::FlatShaded)].programId =
        OpenGLUtil :: loadProgram ( Shader::flatVertexShader   ()
                                  , Shader::flatFragmentShader ()
                                  );
      this->shaderIds [static_cast <int> (RenderMode::Color)].programId =
        OpenGLUtil :: loadProgram ( Shader::simpleVertexShader   ()
                                  , Shader::simpleFragmentShader ()
                                  );

      for (unsigned int i = 0; i < RenderModeUtil :: numRenderModes; i++) {
        ShaderIds *s                = &this->shaderIds [i];
        GLuint     p                = s->programId;
        s->mvpId                    = glGetUniformLocation (p, "mvp");
        s->modelId                  = glGetUniformLocation (p, "model");
        s->colorId                  = glGetUniformLocation (p, "color");
        s->ambientId                = glGetUniformLocation (p, "ambient");
        s->eyePointId               = glGetUniformLocation (p, "eyePoint");
        s->lightIds[0].positionId   = glGetUniformLocation (p, "light1Position");
        s->lightIds[0].colorId      = glGetUniformLocation (p, "light1Color");
        s->lightIds[0].irradianceId = glGetUniformLocation (p, "light1Irradiance");
        s->lightIds[1].positionId   = glGetUniformLocation (p, "light2Position");
        s->lightIds[1].colorId      = glGetUniformLocation (p, "light2Color");
        s->lightIds[1].irradianceId = glGetUniformLocation (p, "light2Irradiance");
      }

      this->setAmbient         (  Config::get<Color>     ("/config/editor/light/ambient"));
      this->setLightPosition   (0,Config::get<glm::vec3> ("/config/editor/light/light1/position"));
      this->setLightColor      (0,Config::get<Color>     ("/config/editor/light/light1/color"));
      this->setLightIrradiance (0,Config::get<float>     ("/config/editor/light/light1/irradiance"));
      this->setLightPosition   (1,Config::get<glm::vec3> ("/config/editor/light/light2/position"));
      this->setLightColor      (1,Config::get<Color>     ("/config/editor/light/light2/color"));
      this->setLightIrradiance (1,Config::get<float>     ("/config/editor/light/light2/irradiance"));

      isInitialized = true;
    }
  }

  void shutdown () {
    for (unsigned int rm = 0; rm < RenderModeUtil :: numRenderModes; rm++)
      OpenGLUtil :: safeDeleteProgram (this->shaderIds[rm].programId);
  }

  void renderInitialize () {
    glClearColor ( this->clearColor.r ()
                 , this->clearColor.g ()
                 , this->clearColor.b (), 0.0f);
        
    glDepthFunc (GL_LEQUAL); 
    glEnable    (GL_CULL_FACE);
    glEnable    (GL_DEPTH_TEST); 
    glClear     (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }

  void setProgram (const RenderMode& renderMode) {
    this->activeShaderIndex = &this->shaderIds[static_cast <int> (renderMode)];
    glUseProgram (this->activeShaderIndex->programId);

    OpenGLUtil :: glUniformVec3 
      (this->activeShaderIndex->ambientId, this->globalUniforms.ambient.vec3 ());

    OpenGLUtil :: glUniformVec3 
      (this->activeShaderIndex->eyePointId, this->globalUniforms.eyePoint);

    for (unsigned int i = 0; i < numLights; i++) {
      OpenGLUtil :: glUniformVec3 
        ( this->activeShaderIndex->lightIds[i].positionId
        , this->globalUniforms.lightUniforms[i].position);
      OpenGLUtil :: glUniformVec3 
        ( this->activeShaderIndex->lightIds[i].colorId
        , this->globalUniforms.lightUniforms[i].color.vec3 ());
      glUniform1f ( this->activeShaderIndex->lightIds[i].irradianceId
                  , this->globalUniforms.lightUniforms[i].irradiance);
    }
  }

  void setMvp (const GLfloat* mvp) {
    assert (this->activeShaderIndex);
    glUniformMatrix4fv (this->activeShaderIndex->mvpId, 1, GL_FALSE, mvp);
  }

  void setModel (const GLfloat* model) {
    assert (this->activeShaderIndex);
    glUniformMatrix4fv (this->activeShaderIndex->modelId, 1, GL_FALSE, model);
  }

  void setColor3 (const Color& c) {
    assert (this->activeShaderIndex);
    OpenGLUtil :: glUniformVec3 (this->activeShaderIndex->colorId, c.vec3 ());
  }

  void setColor4 (const Color& c) {
    assert (this->activeShaderIndex);
    OpenGLUtil :: glUniformVec4 (this->activeShaderIndex->colorId, c.vec4 ());
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

  void updateLights (const Camera& camera) {
    glm::mat3x3 world = glm::mat3x3 (camera.world ());
    glm::vec3   pos1  = Config::get<glm::vec3> ("/config/editor/light/light1/position");
    glm::vec3   pos2  = Config::get<glm::vec3> ("/config/editor/light/light2/position");

    this->setLightPosition (0, world * pos1);
    this->setLightPosition (1, world * pos2);
  }
};

GLOBAL                (Renderer)
DELEGATE_CONSTRUCTOR  (Renderer)
DELEGATE_DESTRUCTOR   (Renderer)

DELEGATE_GLOBAL  (void,Renderer,initialize)
DELEGATE_GLOBAL  (void,Renderer,shutdown)
DELEGATE_GLOBAL  (void,Renderer,renderInitialize)
DELEGATE1_GLOBAL (void,Renderer,setProgram        ,const RenderMode&)
DELEGATE1_GLOBAL (void,Renderer,setMvp            ,const GLfloat*)
DELEGATE1_GLOBAL (void,Renderer,setModel          ,const GLfloat*)
DELEGATE1_GLOBAL (void,Renderer,setColor3         ,const Color&)
DELEGATE1_GLOBAL (void,Renderer,setColor4         ,const Color&)
DELEGATE1_GLOBAL (void,Renderer,setAmbient        ,const Color&)
DELEGATE1_GLOBAL (void,Renderer,setEyePoint       ,const glm::vec3&)
DELEGATE2_GLOBAL (void,Renderer,setLightPosition  ,unsigned int, const glm::vec3&)
DELEGATE2_GLOBAL (void,Renderer,setLightColor     ,unsigned int, const Color&)
DELEGATE2_GLOBAL (void,Renderer,setLightIrradiance,unsigned int, float)
DELEGATE1_GLOBAL (void,Renderer,updateLights      ,const Camera&)
