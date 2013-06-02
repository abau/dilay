#include <stdexcept>
#include <glm/glm.hpp>
#include "renderer.hpp"
#include "config.hpp"
#include "opengl-util.hpp"
#include "rendermode.hpp"
#include "color.hpp"
#include "macro.hpp"
#include "camera.hpp"

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
  GLuint   colorId;
  GLuint   ambientId;
  LightIds lightIds [numLights];

  ShaderIds () {
    this->programId = 0;
    this->mvpId     = 0;
    this->colorId   = 0;
    this->ambientId = 0;
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
};

struct RendererImpl {
  ShaderIds      shaderIds [RenderModeUtil :: numRenderModes];
  ShaderIds*     activeShaderIndex;
  GlobalUniforms globalUniforms;

  RendererImpl () : activeShaderIndex (nullptr) {}

  void initialize () {
    static bool isInitialized = false;

    if ( ! isInitialized ) {
      GLenum err = glewInit ();
      if (err  != GLEW_OK ) {
        std::string e1 = std::string ("Error while initializing glew: ");
        std::string e2 = std::string ((const char*)glewGetErrorString (err));
        throw (std::runtime_error (e1 + e2));
      }

      glClearColor ( Config::get<float> ("/editor/background-color/0")
                   , Config::get<float> ("/editor/background-color/1")
                   , Config::get<float> ("/editor/background-color/2")
                   , 0.0f);
          
      glDepthFunc (GL_LEQUAL); 

      glEnable  (GL_CULL_FACE);
      glEnable  (GL_DEPTH_TEST); 

      this->shaderIds [static_cast <int> (RenderMode::Smooth)].programId =
        OpenGLUtil :: global ().loadProgram ( "shader/smooth-vertex.shader"
                                            , "shader/smooth-fragment.shader" 
                                            );
      this->shaderIds [static_cast <int> (RenderMode::Wireframe)].programId =
        OpenGLUtil :: global ().loadProgram ( "shader/simple-vertex.shader"
                                            , "shader/simple-fragment.shader" 
                                            );
      this->shaderIds [static_cast <int> (RenderMode::Flat)].programId =
        OpenGLUtil :: global ().loadProgram ( "shader/flat-vertex.shader"
                                            , "shader/flat-fragment.shader" 
                                            );

      for (unsigned int i = 0; i < RenderModeUtil :: numRenderModes; i++) {
        ShaderIds *s                = &this->shaderIds [i];
        GLuint     p                = s->programId;
        s->mvpId                    = glGetUniformLocation (p, "mvp");
        s->colorId                  = glGetUniformLocation (p, "color");
        s->ambientId                = glGetUniformLocation (p, "ambient");
        s->lightIds[0].positionId   = glGetUniformLocation (p, "light1Position");
        s->lightIds[0].colorId      = glGetUniformLocation (p, "light1Color");
        s->lightIds[0].irradianceId = glGetUniformLocation (p, "light1Irradiance");
        s->lightIds[1].positionId   = glGetUniformLocation (p, "light2Position");
        s->lightIds[1].colorId      = glGetUniformLocation (p, "light2Color");
        s->lightIds[1].irradianceId = glGetUniformLocation (p, "light2Irradiance");
      }

      this->setAmbient         (  Config::get<Color>     ("/editor/ambient"));
      this->setLightPosition   (0,Config::get<glm::vec3> ("/editor/light1/position"));
      this->setLightColor      (0,Config::get<Color>     ("/editor/light1/color"));
      this->setLightIrradiance (0,Config::get<float>     ("/editor/light1/irradiance"));
      this->setLightPosition   (1,Config::get<glm::vec3> ("/editor/light2/position"));
      this->setLightColor      (1,Config::get<Color>     ("/editor/light2/color"));
      this->setLightIrradiance (1,Config::get<float>     ("/editor/light2/irradiance"));

      isInitialized = true;
    }
  }

  void shutdown () {
    for (unsigned int rm = 0; rm < RenderModeUtil :: numRenderModes; rm++)
      OpenGLUtil :: global ().safeDeleteProgram (this->shaderIds[rm].programId);
  }

  void setProgram (const RenderMode& renderMode) {
    this->activeShaderIndex = &this->shaderIds[static_cast <int> (renderMode)];
    glUseProgram (this->activeShaderIndex->programId);

    OpenGLUtil :: global ().glUniformVec3 
      (this->activeShaderIndex->ambientId, this->globalUniforms.ambient.vec3 ());

    for (unsigned int i = 0; i < numLights; i++) {
      OpenGLUtil :: global ().glUniformVec3 
        ( this->activeShaderIndex->lightIds[i].positionId
        , this->globalUniforms.lightUniforms[i].position);
      OpenGLUtil :: global ().glUniformVec3 
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

  void setColor3 (const Color& c) {
    assert (this->activeShaderIndex);
    OpenGLUtil :: global ().glUniformVec3 (this->activeShaderIndex->colorId, c.vec3 ());
  }

  void setColor4 (const Color& c) {
    assert (this->activeShaderIndex);
    OpenGLUtil :: global ().glUniformVec4 (this->activeShaderIndex->colorId, c.vec4 ());
  }

  void setAmbient (const Color& c) {
    this->globalUniforms.ambient = c;
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
    glm::vec3   pos1  = Config::get<glm::vec3> ("/editor/light1/position");
    glm::vec3   pos2  = Config::get<glm::vec3> ("/editor/light2/position");

    this->setLightPosition (0, world * pos1);
    this->setLightPosition (1, world * pos2);
  }
};

GLOBAL                (Renderer)
DELEGATE_CONSTRUCTOR  (Renderer)
DELEGATE_DESTRUCTOR   (Renderer)

DELEGATE  (void,Renderer,initialize)
DELEGATE  (void,Renderer,shutdown)
DELEGATE1 (void,Renderer,setProgram        ,const RenderMode&)
DELEGATE1 (void,Renderer,setMvp            ,const GLfloat*)
DELEGATE1 (void,Renderer,setColor3         ,const Color&)
DELEGATE1 (void,Renderer,setColor4         ,const Color&)
DELEGATE1 (void,Renderer,setAmbient        ,const Color&)
DELEGATE2 (void,Renderer,setLightPosition  ,unsigned int, const glm::vec3&)
DELEGATE2 (void,Renderer,setLightColor     ,unsigned int, const Color&)
DELEGATE2 (void,Renderer,setLightIrradiance,unsigned int, float)
DELEGATE1 (void,Renderer,updateLights      ,const Camera&)
