#include <QApplication>
#include <QKeyEvent>
#include <glm/glm.hpp>
#include "view-gl-widget.hpp"
#include "renderer.hpp"
#include "view-mouse-movement.hpp"
#include "axis.hpp"
#include "state.hpp"
#include "macro.hpp"
#include "camera.hpp"
#include "winged-util.hpp"
#include "winged-mesh.hpp"
#include "intersection.hpp"
#include "ray.hpp"
#include "cursor.hpp"
#include "winged-face.hpp"
#include "tool.hpp"

struct GLWidgetImpl {
  MouseMovement mouseMovement;
  Axis          axis;
};

GLWidget :: GLWidget (const QGLFormat& format) : QGLWidget (format) 
                                               , impl (new GLWidgetImpl)
                                               {}


GLWidget :: ~GLWidget () {
  delete this->impl;
}

void GLWidget :: initializeGL () {
  Renderer :: initialize ();
  State    :: initialize ();

  this->impl->axis.initialize ();
  this->setMouseTracking (true);

  Renderer :: updateLights (State :: camera ());
}

void GLWidget :: paintGL () {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  State :: render ();
  this->impl->axis.render ();
}

void GLWidget :: resizeGL (int w, int h) {
  State :: camera ().updateResolution (w,h);
  glViewport (0,0,w, h < 1 ? 1 : h );
}

void GLWidget :: keyPressEvent (QKeyEvent* e) {
  switch (e->key()) {
    case Qt::Key_Escape:
      QCoreApplication::instance()->quit();
      break;
    case Qt::Key_W:
      State :: mesh ().toggleRenderMode ();
      this->update ();
      break;
    case Qt::Key_I:
      WingedUtil :: printStatistics (State :: mesh ());
      break;
    default:
      QGLWidget::keyPressEvent (e);
  }
}

void GLWidget :: mouseMoveEvent (QMouseEvent* e) {
  if (e->buttons () == Qt :: NoButton) {
    WingedMesh& mesh = State :: mesh ();
    int reversedY    = State :: camera ().resolutionHeight () - e->y ();
    Ray ray          = State :: camera ().getRay ( e->x () , reversedY);

    FaceIntersection intersection;
    mesh.intersectRay (ray, intersection);

    if (intersection.isIntersection ()) {
      glm::vec3 pos    = intersection.position ();
      glm::vec3 normal = intersection.face ()->normal (mesh);

      State :: cursor ().enable      ();
      State :: cursor ().setPosition (pos);
      State :: cursor ().setNormal   (normal);
      this->update ();
    }
    else if (State :: cursor ().isEnabled ()) {
      State :: cursor ().disable ();
      this->update ();
    }
  }
  if (e->buttons () == Qt :: MiddleButton) {
    this->impl->mouseMovement.update (e->pos ());
    State    :: camera ().verticalRotation   (this->impl->mouseMovement.dX ());
    State    :: camera ().horizontalRotation (this->impl->mouseMovement.dY ());
    Renderer :: updateLights (State :: camera ());

    this->update ();
  }
}

void GLWidget :: mousePressEvent (QMouseEvent* e) {
  if (e->buttons () == Qt :: LeftButton) {
    int reversedY = State :: camera ().resolutionHeight () - e->y ();
    if (Tool :: click (e->x (), reversedY))
      this->update ();
  }
}

void GLWidget :: mouseReleaseEvent (QMouseEvent*) {
  this->impl->mouseMovement.invalidate ();
}

void GLWidget :: resizeEvent (QResizeEvent* e) {
  State :: camera ().updateResolution ( e->size ().width  ()
                                      , e->size ().height ());
}

void GLWidget :: wheelEvent (QWheelEvent* e) {
  if (e->orientation () == Qt :: Vertical) {
    if (e->delta () > 0)
      State :: camera ().stepAlongGaze (true);
    else if (e->delta () < 0)
      State :: camera ().stepAlongGaze (false);
  }
  this->update ();
}
