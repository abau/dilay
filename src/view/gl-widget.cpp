#include <GL/glew.h>
#include "gl-widget.hpp"
#include "opengl.hpp"
#include "state.hpp"

GLWidget :: GLWidget (const QGLFormat& format) : QGLWidget (format) {}

void GLWidget :: initializeGL () {
  OpenGL :: initialize ();
}

void GLWidget :: paintGL () {
  glClear( GL_COLOR_BUFFER_BIT );
  glUseProgram(OpenGL :: programId ());
  //State :: global ().mesh () -> render ();
}

void GLWidget :: resizeGL (int w, int h) {
  State :: global ().camera ().updateResolution (w,h);
  glViewport (0,0,w, h < 1 ? 1 : h );
}

void GLWidget :: keyPressEvent (QKeyEvent* e)
{
  /*
    switch ( e->key() )
    {
        case Qt::Key_Escape:
            QCoreApplication::instance()->quit();
            break;
 
        default:
            QGLWidget::keyPressEvent( e );
    }
    */
}

void GLWidget :: mouseMoveEvent (QMouseEvent* e) {
  this->_mouseMovement.update (e->pos ());
}

void GLWidget :: mouseReleaseEvent (QMouseEvent* _) {
  this->_mouseMovement.invalidate ();
}
