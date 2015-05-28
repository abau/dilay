#include <QApplication>
#include <QKeyEvent>
#include <QPainter>
#include <glm/glm.hpp>
#include <memory>
#include "camera.hpp"
#include "history.hpp"
#include "opengl.hpp"
#include "primitive/ray.hpp"
#include "render-mode.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool.hpp"
#include "tool/move-camera.hpp"
#include "view/axis.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/util.hpp"
#include "winged/mesh.hpp"

struct ViewGlWidget::Impl {
  typedef std::unique_ptr <State>    StatePtr;
  typedef std::unique_ptr <ViewAxis> AxisPtr;

  ViewGlWidget*   self;
  ViewMainWindow& mainWindow;
  Config&         config;
  Cache&          cache;
  ToolMoveCamera  toolMoveCamera;
  AxisPtr         axis;
  StatePtr       _state;

  Impl (ViewGlWidget* s, ViewMainWindow& mW, Config& cfg, Cache& cch) 
    : self           (s)
    , mainWindow     (mW)
    , config         (cfg)
    , cache          (cch)
    , toolMoveCamera (cfg)
    , axis           (nullptr)
    ,_state          (nullptr)
  {
    this->self->setAutoFillBackground (false);
  }

  ~Impl () {
    this->self->makeCurrent ();

    this-> axis .reset (nullptr);
    this->_state.reset (nullptr);

    this->self->doneCurrent ();
  }

  State& state () {
    assert (this->_state);
    return *this->_state;
  }

  glm::ivec2 cursorPosition () {
    return ViewUtil::toIVec2 (this->self->mapFromGlobal (QCursor::pos ()));
  }

  void initializeGL () {
    OpenGL::initializeFunctions ();

    this-> axis .reset (new ViewAxis (this->config));
    this->_state.reset (new State (this->mainWindow, this->config, this->cache));

    this->self->setMouseTracking (true);
    this->self->setFocus         ();
  }

  void paintGL () {
    QPainter painter (this->self);
    painter.beginNativePainting ();

    this->state ().camera ().renderer ().setupRendering ();
    this->state ().scene  ().render (this->state ().camera ());

    if (this->state ().hasTool ()) {
      this->state ().tool ().render ();
    }
    this->axis->render (this->state ().camera ());

    OpenGL::glDisable (OpenGL::DepthTest ()); 
    painter.endNativePainting ();

    this->axis->render (this->state ().camera (), painter);
    this->mainWindow.showNumFaces (this->state ().scene ().numFaces ());
  }

  void resizeGL (int w, int h) {
    this->state ().camera ().updateResolution (glm::uvec2 (w,h));
  }

  void keyPressEvent (QKeyEvent* e) {
    const int                   key     = e->key ();
    const Qt::KeyboardModifiers mod     = e->modifiers ();
    const bool                  hasTool = this->state ().hasTool ();

    if (key == Qt::Key_W) {
      RenderMode mode = this->state ().scene ().commonRenderMode ();

      if (mod == Qt::ControlModifier) {
        if (mode.smoothShading ()) {
          mode.flatShading (true);
        }
        else if (mode.flatShading ()) {
          mode.smoothShading (true);
        }
      }
      else if (mod == Qt::NoModifier) {
        mode.renderWireframe (! mode.renderWireframe ());
      }
      this->state ().scene ().commonRenderMode (mode);
      this->self->update ();
    }
    else if (key == Qt::Key_I) {
      this->state ().scene ().printStatistics (false);
    }
    else if (key == Qt::Key_Z && mod == Qt::ControlModifier) {
      this->state ().undo ();
      this->self->update ();
    }
    else if (key == Qt::Key_Y && mod == Qt::ControlModifier) {
      this->state ().redo ();
      this->self->update ();
    }
    else if (hasTool) {
      if (key == Qt::Key_Escape || key == Qt::Key_Enter) {
        this->state ().resetTool ();
        this->self->update ();
      }
    }
    else if (key == Qt::Key_Escape) {
      QCoreApplication::instance()->quit();
    }
    else {
      this->self->QOpenGLWidget::keyPressEvent (e);
    }
  }

  void mouseMoveEvent (QMouseEvent* e) {
    this->self->setFocus (Qt::MouseFocusReason);

    if (e->buttons () == Qt::MiddleButton) {
      this->toolMoveCamera.mouseMoveEvent (this->state (), *e);
    }
    else if (this->state ().hasTool ()) {
      this->state ().handleToolResponse (this->state ().tool ().mouseMoveEvent (*e));
    }
  }

  void mousePressEvent (QMouseEvent* e) {
    if (e->button () == Qt::MiddleButton) {
      this->toolMoveCamera.mousePressEvent (this->state (), *e);
    }
    else if (this->state ().hasTool ()) {
      this->state ().handleToolResponse (this->state ().tool ().mousePressEvent (*e));
    }
  }

  void mouseReleaseEvent (QMouseEvent* e) {
    if (this->state ().hasTool ()) {
      this->state ().handleToolResponse (this->state ().tool ().mouseReleaseEvent (*e));
    }
  }

  void wheelEvent (QWheelEvent* e) {
    if (e->modifiers () == Qt::NoModifier) {
      this->toolMoveCamera.wheelEvent (this->state (), *e);
    }
    else if (this->state ().hasTool ()) {
      this->state ().handleToolResponse (this->state ().tool ().wheelEvent (*e));
    }
  }
};

DELEGATE3_BIG2_SELF (ViewGlWidget, ViewMainWindow&, Config&, Cache&)
DELEGATE  (State&    , ViewGlWidget, state)
DELEGATE  (glm::ivec2, ViewGlWidget, cursorPosition)
DELEGATE  (void      , ViewGlWidget, initializeGL)
DELEGATE2 (void      , ViewGlWidget, resizeGL, int, int)
DELEGATE  (void      , ViewGlWidget, paintGL)
DELEGATE1 (void      , ViewGlWidget, keyPressEvent    , QKeyEvent*)
DELEGATE1 (void      , ViewGlWidget, mouseMoveEvent   , QMouseEvent*)
DELEGATE1 (void      , ViewGlWidget, mousePressEvent  , QMouseEvent*)
DELEGATE1 (void      , ViewGlWidget, mouseReleaseEvent, QMouseEvent*)
DELEGATE1 (void      , ViewGlWidget, wheelEvent       , QWheelEvent*)
