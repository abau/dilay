#include <GL/glew.h>
#include <GL/gl.h>
#include <QApplication>
#include <QGLFormat>
#include "view-gl-widget.hpp"
#include "state.hpp"

int main(int argv, char **args) {
  QApplication app(argv, args);

  QGLFormat glFormat;
  glFormat.setVersion         (2,1);
  glFormat.setProfile         (QGLFormat::CoreProfile); 
  glFormat.setDepth           (true); 
  glFormat.setDepthBufferSize (24); 

  GLWidget w (glFormat);
  w.show ();

  return app.exec();
}

/*
  State :: global ().setMesh(WingedMesh::triangle (glm::vec3 (0,0,0), glm::vec3 (0,0,1), glm::vec3 (1,0,0)));
  State :: global ().mesh ()->bufferData ();

  // Main loop
  while (State :: global ().isRunning ()) {
    if (glfwGetWindowParam( GLFW_OPENED ) == GL_FALSE) {
      break;
    }

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(State :: global ().programId ());

    State :: global ().mesh ()->render ();

    glfwSwapBuffers();
  }
  OpenGL :: shutdown ();
  exit (EXIT_SUCCESS);
}

void GLFWCALL handleKey (int key, int action) {
  switch (action) {
    case GLFW_PRESS:
      switch (key) {
        case GLFW_KEY_ESC: {
          State :: global ().terminate ();
          break;
        }
        case 'I': {
          State :: global ().mesh ()->printStats ();
          break;
        }
        case 'S': {
          EdgeIterator eIt = State :: global ().mesh ()->edgeIterator();

          while (eIt.linkedElement ()->hasNext ()) { eIt.next (); }

          AdaptiveMesh :: splitEdge (*State :: global ().mesh (),*eIt.linkedElement ());
          State :: global ().mesh ()->rebuildIndices ();
          State :: global ().mesh ()->bufferData ();
          break;
        }
        case 'W': {
          OpenGL :: toggleWireframe ();
          break;
        }
      }
      break;
  }
}
*/
