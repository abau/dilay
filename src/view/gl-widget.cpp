#include <GL/glew.h>
#include "gl-widget.hpp"
#include "opengl.hpp"
#include "state.hpp"

#include "ray.hpp"
#include "intersection.hpp"
#include "maybe.hpp"
#include "adaptive-mesh.hpp"

GLWidget :: GLWidget (const QGLFormat& format) : QGLWidget (format) {}

void GLWidget :: initializeGL () {
  OpenGL :: initialize ();
}

void GLWidget :: paintGL () {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(OpenGL :: programId ());
  State :: global ().mesh ().render ();
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
  if (e->buttons () == Qt :: MiddleButton) {
    this->_mouseMovement.update (e->pos ());
    State :: global ().camera ().verticalRotation   (this->_mouseMovement.dX ());
    State :: global ().camera ().horizontalRotation (this->_mouseMovement.dY ());
    this->update ();
  }
}

void GLWidget :: mousePressEvent (QMouseEvent* e) {
  if (e->buttons () == Qt :: LeftButton) {
    int reversedY = State :: global ().camera ().resolutionHeight () - e->y ();
    Ray ray = State :: global ().camera ().getRay (e->x (), reversedY);
    Maybe <FaceIntersection> i = State :: global ().mesh ().intersectRay (ray);
    if (i.isDefined ()) {
      AdaptiveMesh :: splitFaceRegular (State :: global ().mesh (),i.data().face());
      State :: global ().mesh ().rebuildIndices ();
      State :: global ().mesh ().bufferData ();
      this->update ();
    }
  }
}

void GLWidget :: mouseReleaseEvent (QMouseEvent* _) {
  this->_mouseMovement.invalidate ();
}

void GLWidget :: resizeEvent (QResizeEvent* e) {
  State :: global ().camera ().updateResolution ( e->size ().width  ()
                                                , e->size ().height ());
}

void GLWidget :: wheelEvent (QWheelEvent* e) {
  if (e->orientation () == Qt :: Vertical) {
    if (e->delta () > 0)
      State :: global ().camera ().stepAlongGaze (true);
    else if (e->delta () < 0)
      State :: global ().camera ().stepAlongGaze (false);
  }
  this->update ();
}
