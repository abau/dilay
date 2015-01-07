#include <QApplication>
#include <QKeyEvent>
#include <QPainter>
#include <glm/glm.hpp>
#include <memory>
#include "camera.hpp"
#include "history.hpp"
#include "opengl.hpp"
#include "primitive/ray.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "selection-mode.hpp"
#include "state.hpp"
#include "tool.hpp"
#include "tool/move-camera.hpp"
#include "view/axis.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/menu/winged-mesh.hpp"
#include "view/menu/no-selection.hpp"
#include "view/properties/selection.hpp"
#include "view/properties/widget.hpp"
#include "view/tool/menu-parameters.hpp"
#include "view/util.hpp"

struct ViewGlWidget::Impl {
  typedef std::unique_ptr <State>    StatePtr;
  typedef std::unique_ptr <ViewAxis> AxisPtr;

  ViewGlWidget*   self;
  ViewMainWindow& mainWindow;
  Config&         config;
  ToolMoveCamera  toolMoveCamera;
  AxisPtr         axis;
  StatePtr        state;

  Impl (ViewGlWidget* s, ViewMainWindow& mW, Config& c) 
    : self           (s)
    , mainWindow     (mW)
    , config         (c)
    , toolMoveCamera (c)
    , axis           (nullptr)
    , state          (nullptr)
  {
    this->self->setAutoFillBackground (false);
  }

  ~Impl () {
    this->self->makeCurrent ();

    this->axis .reset (nullptr);
    this->state.reset (nullptr);

    this->self->doneCurrent ();
  }

  glm::ivec2 cursorPosition () {
    return ViewUtil::toIVec2 (this->self->mapFromGlobal (QCursor::pos ()));
  }

  void selectIntersection () {
    this->selectIntersection (this->cursorPosition ());
  }

  void selectIntersection (const glm::ivec2& pos) {
    if (this->state->scene ().selectIntersection (this->state->camera ().ray (pos))) {
      this->mainWindow.showNumSelections (this->state->scene ().numSelections ());
      this->self->update ();
    }
  }

  void initializeGL () {
    OpenGL  ::initializeFunctions ();

    this->axis .reset (new ViewAxis (this->config));
    this->state.reset (new State (this->mainWindow, this->config));

    this->self->setMouseTracking (true);
    this->self->setFocus         ();

    QObject::connect 
      ( &this->mainWindow.properties ().selection ()
      , &ViewPropertiesSelection::selectionModeChanged 
      , [this] (SelectionMode m) { 
          this->state->scene ().changeSelectionMode (m);
          this->mainWindow.showNumSelections (this->state->scene ().numSelections ());
          this->self->update (); 
      });
  }

  void paintGL () {
    QPainter painter (this->self);
    painter.beginNativePainting ();

    this->state->renderer ().setupRendering ();
    this->state->scene    ().render <WingedMesh> (this->state->camera ());

    if (this->state->hasTool ()) {
      this->state->tool ().render ();
    }
    this->axis->render (this->state->camera ());

    OpenGL::glDisable (OpenGL::DepthTest ()); 
    painter.endNativePainting ();

    this->axis->render (this->state->camera (), painter);
  }

  void resizeGL (int w, int h) {
    this->state->camera ().updateResolution (glm::uvec2 (w,h));
  }

  void keyPressEvent (QKeyEvent* e) {
    const int                   key     = e->key ();
    const Qt::KeyboardModifiers mod     = e->modifiers ();
    const bool                  hasTool = this->state->hasTool ();

    if (key == Qt::Key_W) {
      if (mod == Qt::ControlModifier) {
        this->state->scene ().toggleRenderMode <WingedMesh> ();
        this->self->update ();
      }
      else if (mod == Qt::NoModifier) {
        this->state->scene ().toggleRenderWireframe <WingedMesh> ();
        this->self->update ();
      }
    }
    else if (key == Qt::Key_I) {
      this->state->scene ().printStatistics (false);
    }
    else if (key == Qt::Key_Z && mod == Qt::ControlModifier
         && (hasTool == false || this->state->tool ().allowUndo ()))
    {
      this->state->history ().undo ();
      this->self->update ();
    }
    else if (key == Qt::Key_Y && mod == Qt::ControlModifier
         && (hasTool == false || this->state->tool ().allowRedo ()))
    {
      this->state->history ().redo ();
      this->self->update ();
    }
    else if (hasTool) {
      if (key == Qt::Key_Escape || key == Qt::Key_Enter) {
        this->state->tool    ().close ();
        this->state->setTool (nullptr);
        this->self->update   ();
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
      this->toolMoveCamera.mouseMoveEvent (*this->state, *e);
    }
    else if (this->state->hasTool ()) {
      this->state->handleToolResponse (this->state->tool ().mouseMoveEvent (*e));
    }
  }

  void mousePressEvent (QMouseEvent* e) {
    if (e->button () == Qt::MiddleButton) {
      this->toolMoveCamera.mousePressEvent (*this->state, *e);
    }
    else if (this->state->hasTool ()) {
      this->state->handleToolResponse (this->state->tool ().mousePressEvent (*e));
    }
  }

  void mouseReleaseEvent (QMouseEvent* e) {
    if (this->state->hasTool ()) {
      this->state->handleToolResponse (this->state->tool ().mouseReleaseEvent (*e));
    }
    else if (e->button () == Qt::LeftButton) {
      this->selectIntersection (ViewUtil::toIVec2 (*e));
    }
    else if (e->button () == Qt::RightButton) {
      glm::ivec2 pos = ViewUtil::toIVec2 (*e);

      if (this->state->scene ().numSelections () == 0) {
        this->selectIntersection (pos);
      }
      if (this->state->scene ().numSelections () == 0) {
        ViewMenuNoSelection menu (*this->state, pos);
        menu.exec (e->globalPos ());
      }
      else {
        switch (this->state->scene ().selectionMode ()) {
          case SelectionMode::WingedMesh: {
            ViewMenuWingedMesh menu (*this->state, pos);
            menu.exec (e->globalPos ());
            break;
          }
        }
      }
    }
  }

  void wheelEvent (QWheelEvent* e) {
    if (e->modifiers () == Qt::NoModifier) {
      this->toolMoveCamera.wheelEvent (*this->state, *e);
    }
    else if (this->state->hasTool ()) {
      this->state->handleToolResponse (this->state->tool ().wheelEvent (*e));
    }
  }
};

DELEGATE2_BIG2_SELF (ViewGlWidget, ViewMainWindow&, Config&)
DELEGATE  (glm::ivec2, ViewGlWidget, cursorPosition)
DELEGATE  (void      , ViewGlWidget, selectIntersection)
DELEGATE1 (void      , ViewGlWidget, selectIntersection, const glm::ivec2&)
DELEGATE  (void      , ViewGlWidget, initializeGL)
DELEGATE2 (void      , ViewGlWidget, resizeGL, int, int)
DELEGATE  (void      , ViewGlWidget, paintGL)
//DELEGATE1 (void      , ViewGlWidget, paintEvent,QPaintEvent*)
DELEGATE1 (void      , ViewGlWidget, keyPressEvent    , QKeyEvent*)
DELEGATE1 (void      , ViewGlWidget, mouseMoveEvent   , QMouseEvent*)
DELEGATE1 (void      , ViewGlWidget, mousePressEvent  , QMouseEvent*)
DELEGATE1 (void      , ViewGlWidget, mouseReleaseEvent, QMouseEvent*)
DELEGATE1 (void      , ViewGlWidget, wheelEvent       , QWheelEvent*)
