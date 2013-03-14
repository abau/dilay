#include <glog/logging.h>
#include <stdexcept>
#include <glm/glm.hpp>
#include "renderer.hpp"
#include "config.hpp"
#include "opengl-util.hpp"
#include "rendermode.hpp"
#include "color.hpp"
#include "view-light.hpp"
#include "macro.hpp"

struct RendererImpl {
  GLuint programIds [RenderModeUtil :: numRenderModes];
  GLuint mvpId;
  GLuint colorId;
  GLuint light1PositionId;
  GLuint light1ColorId;
  GLuint light1IrradianceId;

  void initialize () {
    static bool isInitialized = false;

    if ( ! isInitialized ) {
      LOG (INFO) << "Initializing renderer";
      GLenum err = glewInit ();
      if (err  != GLEW_OK ) {
        std::string e1 = std::string ("Error while initializing glew: ");
        std::string e2 = std::string ((const char*)glewGetErrorString (err));
        LOG(ERROR) << (e1 + e2);
        throw (std::runtime_error (e1 + e2));
      }

      glClearColor ( Config::global().get<float> ({"editor","background-color","red"})
                   , Config::global().get<float> ({"editor","background-color","green"})
                   , Config::global().get<float> ({"editor","background-color","blue"})
                   , 0.0f);
          
      glDepthFunc (GL_LEQUAL); 

      glEnable  (GL_CULL_FACE);
      glEnable  (GL_DEPTH_TEST); 

      this->programIds [RenderSmooth] = 
        OpenGLUtil :: global ().loadProgram ( "shader/smooth-vertex.shader"
                                            , "shader/smooth-fragment.shader" 
                                            );
      this->programIds [RenderWireframe] = 
        OpenGLUtil :: global ().loadProgram ( "shader/simple-vertex.shader"
                                            , "shader/simple-fragment.shader" 
                                            );
      this->programIds [RenderFlat] = 
        OpenGLUtil :: global ().loadProgram ( "shader/flat-vertex.shader"
                                            , "shader/flat-fragment.shader" 
                                            );
      isInitialized = true;
    }
  }

  void shutdown () {
    for (int rm = 0; rm < RenderModeUtil :: numRenderModes; rm++)
      OpenGLUtil :: global ().safeDeleteProgram (this->programIds[rm]);
  }

  void setColor3 (const Color& c) {
    OpenGLUtil :: global ().glUniformVec3 (this->colorId, c.vec3 ());
  }

  void setColor4 (const Color& c) {
    OpenGLUtil :: global ().glUniformVec4 (this->colorId, c.vec4 ());
  }

  void setMvp (const GLfloat* mvp) {
    glUniformMatrix4fv (this->mvpId, 1, GL_FALSE, mvp);
  }

  void setProgram (const RenderMode& renderMode) {
    GLuint programId         = this->programIds [renderMode];
    glUseProgram (programId);
    this->mvpId              = glGetUniformLocation (programId, "mvp");
    this->colorId            = glGetUniformLocation (programId, "color");
    this->light1PositionId   = glGetUniformLocation (programId, "light1Position");
    this->light1ColorId      = glGetUniformLocation (programId, "light1Color");
    this->light1IrradianceId = glGetUniformLocation (programId, "light1Irradiance");
  }

  void setViewLight1 (const ViewLight& light) {
    OpenGLUtil :: global ().glUniformVec3 (
        this->light1PositionId, light.position ());
    OpenGLUtil :: global ().glUniformVec3 (
        this->light1ColorId, light.color ().vec3 ());
    glUniform1f (this->light1IrradianceId, light.irradiance ());
  }
};

GLOBAL                (Renderer)
DELEGATE_CONSTRUCTOR  (Renderer)
DELEGATE_DESTRUCTOR   (Renderer)

DELEGATE  (void,Renderer,initialize)
DELEGATE  (void,Renderer,shutdown)
DELEGATE1 (void,Renderer,setColor3    ,const Color&)
DELEGATE1 (void,Renderer,setColor4    ,const Color&)
DELEGATE1 (void,Renderer,setMvp       ,const GLfloat*)
DELEGATE1 (void,Renderer,setProgram   ,const RenderMode&)
DELEGATE1 (void,Renderer,setViewLight1,const ViewLight&)
