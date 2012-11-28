#include <GL/glew.h>
#include <GL/gl.h>
#include <QApplication>
#include <QGLFormat>
#include "view/gl-widget.hpp"
#include "state.hpp"

int main(int argv, char **args) {
  QApplication app(argv, args);

  QGLFormat glFormat;
  glFormat.setVersion( 2, 1 );
  glFormat.setProfile( QGLFormat::CoreProfile ); 

  GLWidget w (glFormat);
  w.show ();

  State :: global ().initialize  ();
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

void GLFWCALL handleMousePos (int x, int y) {
  static int currentX = x;
  static int currentY = y;

  if (glfwGetMouseButton (GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
    if (x > currentX)
      State :: global ().camera ().verticalRotation (currentX - x);
    else if (x < currentX)
      State :: global ().camera ().verticalRotation (currentX - x);

    if (y > currentY)
      State :: global ().camera ().horizontalRotation (currentY - y);
    else if (y < currentY)
      State :: global ().camera ().horizontalRotation (currentY - y);
  }
  currentX = x;
  currentY = y;
}

void GLFWCALL handleMouseWheel (int pos) {
  static int currentPos = 0;
  if (pos > currentPos)
    State :: global ().camera ().stepAlongGaze (true);
  else if (pos < currentPos)
    State :: global ().camera ().stepAlongGaze (false);
  currentPos = pos;
}

void GLFWCALL handleMouseButton (int button, int action) {
  switch (action) {
    case GLFW_PRESS:
      switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT: {
          int x,y;
          glfwGetMousePos (&x,&y);
          int reversedY = State :: global ().camera ().resolutionHeight () - y;
          Ray ray = State :: global ().camera ().getRay (x,reversedY);
          Maybe <FaceIntersection> i = State :: global ().mesh ()->intersectRay (ray);
          if (i.isDefined ()) 
            AdaptiveMesh :: splitFaceRegular (*State :: global ().mesh (),i.data().face());
            State :: global ().mesh ()->rebuildIndices ();
            State :: global ().mesh ()->bufferData ();

          break;
        }
      }
      break;
  }
}
*/
