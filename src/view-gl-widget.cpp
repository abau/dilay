#include <QApplication>
#include "view-gl-widget.hpp"
#include "opengl.hpp"
#include "state.hpp"

#include "ray.hpp"
#include "intersection.hpp"
#include "maybe.hpp"
#include "winged-mesh-util.hpp"
#include "subdivision-butterfly.hpp"

GLWidget :: GLWidget (const QGLFormat& format) : QGLWidget (format) {}

void GLWidget :: initializeGL () {
  this->setMouseTracking (true);
  OpenGL :: initialize ();

  this->_axis.initialize ();
  State :: global ().initialize ();
}

void GLWidget :: paintGL () {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  State :: global ().render ();
  this->_axis.render ();
}

void GLWidget :: resizeGL (int w, int h) {
  State :: global ().camera ().updateResolution (w,h);
  glViewport (0,0,w, h < 1 ? 1 : h );
}

void GLWidget :: keyPressEvent (QKeyEvent* e) {
  switch (e->key()) {
    case Qt::Key_Escape:
      QCoreApplication::instance()->quit();
      break;
    case Qt::Key_W:
      State :: global ().mesh ().toggleRenderMode ();
      this->update ();
      break;
    case Qt::Key_I:
      WingedMeshUtil :: printStatistics (State :: global ().mesh ());
      break;
    default:
      QGLWidget::keyPressEvent (e);
  }
}

void GLWidget :: mouseMoveEvent (QMouseEvent* e) {
  if (e->buttons () == Qt :: NoButton) {
    int reversedY = State :: global ().camera ().resolutionHeight () - e->y ();
    Ray ray = State :: global ().camera ().getRay (e->x (), reversedY);
    Maybe <FaceIntersection> i = State :: global ().mesh ().intersectRay (ray);

    if (i.isDefined ()) {
      State :: global ().cursor ().setPosition (i.data ().position ());
      this->update ();
    }
  }
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
      SubdivButterfly :: subdiv (State :: global ().mesh ());
      State :: global ().mesh ().rebuildIndices ();
      State :: global ().mesh ().rebuildNormals ();
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
