#include <QApplication>
#include <QKeyEvent>
#include <glm/glm.hpp>
#include "view/gl-widget.hpp"
#include "renderer.hpp"
#include "view/mouse-movement.hpp"
#include "state.hpp"
#include "macro.hpp"
#include "camera.hpp"
#include "winged/util.hpp"
#include "intersection.hpp"
#include "ray.hpp"
#include "cursor.hpp"
#include "winged/face.hpp"
#include "tool/carve.hpp"
#include "tool/rotate.hpp"
#include "history.hpp"
#include "scene.hpp"
#include "axis.hpp"

struct ViewGlWidget :: Impl {
  ViewGlWidget* self;
  Axis          axis;

  Impl (ViewGlWidget* s) : self (s) {}

  void initializeGL () {
    Renderer :: initialize ();
    State    :: initialize ();

    this->axis.initialize ();
    this->self->setMouseTracking (true);

    Renderer :: updateLights (State :: camera ());
  }

  void paintGL () {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    State :: render ();
    this->axis.render ();
  }

  void resizeGL (int w, int h) {
    State :: camera ().updateResolution (glm::uvec2 (w,h));
    glViewport (0,0,w, h < 1 ? 1 : h );
  }

  void keyPressEvent (QKeyEvent* e) {
    switch (e->key()) {
      case Qt::Key_Escape:
        QCoreApplication::instance()->quit();
        break;
        /*
      case Qt::Key_W:
        State :: mesh ().toggleRenderMode ();
        this->update ();
        break;
      case Qt::Key_I:
        WingedUtil :: printStatistics (State :: mesh ());
        break;
        */
      case Qt::Key_Z:
        if (e->modifiers () == Qt::ControlModifier) {
          State :: history ().undo ();
          this->self->update ();
        }
        else
          this->self->QGLWidget::keyPressEvent (e);
        break;
      case Qt::Key_Y:
        if (e->modifiers () == Qt::ControlModifier) {
          State :: history ().redo ();
          this->self->update ();
        }
        else
          this->self->QGLWidget::keyPressEvent (e);
        break;
      default:
        this->self->QGLWidget::keyPressEvent (e);
    }
  }

  void mouseMoveEvent (QMouseEvent* e) {
    State :: mouseMovement ().update (e->pos ());
    if (e->buttons () == Qt :: NoButton) {
      Ray ray = State :: camera ().getRay (glm::uvec2 (e->x (), e->y ()));

      FaceIntersection intersection;
      State :: scene ().intersectRay (ray, intersection);

      if (intersection.isIntersection ()) {
        glm::vec3 pos    = intersection.position ();
        glm::vec3 normal = intersection.face ().normal (intersection.mesh ());

        State :: cursor ().enable      ();
        State :: cursor ().setPosition (pos);
        State :: cursor ().setNormal   (normal);
        this->self->update ();
      }
      else if (State :: cursor ().isEnabled ()) {
        State :: cursor ().disable ();
        this->self->update ();
      }
    }
    if (e->buttons () == Qt :: MiddleButton) {
      ToolRotate :: run ();
      this->self->update ();
    }
  }

  void mousePressEvent (QMouseEvent* e) {
    if (e->buttons () == Qt :: LeftButton) {
      if (ToolCarve :: click ())
        this->self->update ();
    }
  }

  void mouseReleaseEvent (QMouseEvent*) {
    State :: mouseMovement ().invalidate ();
  }

  void resizeEvent (QResizeEvent* e) {
    State :: camera ().updateResolution (glm::uvec2 ( e->size ().width  ()
                                                    , e->size ().height ()));
  }

  void wheelEvent (QWheelEvent* e) {
    if (e->orientation () == Qt :: Vertical) {
      if (e->delta () > 0)
        State :: camera ().stepAlongGaze (true);
      else if (e->delta () < 0)
        State :: camera ().stepAlongGaze (false);
    }
    this->self->update ();
  }
};

ViewGlWidget :: ViewGlWidget (const QGLFormat& format) : QGLWidget (format) {
  this->impl = new Impl (this);
}

DELEGATE_DESTRUCTOR (ViewGlWidget)

DELEGATE  (void, ViewGlWidget, initializeGL)
DELEGATE2 (void, ViewGlWidget, resizeGL         , int, int)
DELEGATE  (void, ViewGlWidget, paintGL)
DELEGATE1 (void, ViewGlWidget, keyPressEvent    , QKeyEvent*)
DELEGATE1 (void, ViewGlWidget, mouseMoveEvent   , QMouseEvent*)
DELEGATE1 (void, ViewGlWidget, mousePressEvent  , QMouseEvent*)
DELEGATE1 (void, ViewGlWidget, mouseReleaseEvent, QMouseEvent*)
DELEGATE1 (void, ViewGlWidget, resizeEvent      , QResizeEvent*)
DELEGATE1 (void, ViewGlWidget, wheelEvent       , QWheelEvent*)
