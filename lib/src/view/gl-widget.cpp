#include <QApplication>
#include <QKeyEvent>
#include <glm/glm.hpp>
#include <memory>
#include "camera.hpp"
#include "history.hpp"
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
  ViewGlWidget*   self;
  ViewMainWindow& mainWindow;
  ViewAxis        axis;
  ToolMoveCamera  toolMoveCamera;

  Impl (ViewGlWidget* s, ViewMainWindow& mW) 
    : self       (s)
    , mainWindow (mW) 
  {
    this->self->setAutoFillBackground (false);
  }

  ~Impl () {
    State::setTool (nullptr);
  }

  glm::ivec2 cursorPosition () {
    return ViewUtil::toIVec2 (this->self->mapFromGlobal (QCursor::pos ()));
  }

  void selectIntersection () {
    this->selectIntersection (this->cursorPosition ());
  }

  void selectIntersection (const glm::ivec2& pos) {
    if (State::scene ().selectIntersection (State::camera ().ray (pos))) {
      this->mainWindow.showNumSelections (State::scene ().numSelections ());
      this->self->update ();
    }
  }

  void initializeGL () {
    Renderer::initialize ();
    State   ::initialize (this->mainWindow);

    this->axis.initialize        ();
    this->self->setMouseTracking (true);
    this->self->setFocus         ();

    Renderer::updateLights (State::camera ());

    QObject::connect 
      ( &this->mainWindow.properties ().selection ()
      , &ViewPropertiesSelection::selectionModeChanged 
      , [this] (SelectionMode m) { 
          State::scene ().changeSelectionMode (m);
          this->mainWindow.showNumSelections (State::scene ().numSelections ());
          this->self->update (); 
      });
  }

  void paintEvent (QPaintEvent*) {
    this->self->makeCurrent ();

    Renderer::renderInitialize ();

    State::scene ().render <WingedMesh> ();

    if (State::hasTool ()) {
      State::tool ().render ();
    }
    this->axis.render ();

    QPainter painter (this->self);

    this->axis.render (painter);

    painter.end ();
  }

  void resizeGL (int w, int h) {
    State::camera ().updateResolution (glm::uvec2 (w,h));
  }

  void keyPressEvent (QKeyEvent* e) {
    const int                   key = e->key ();
    const Qt::KeyboardModifiers mod = e->modifiers ();

    if (key == Qt::Key_W) {
      State :: scene ().toggleRenderMode <WingedMesh> ();
      this->self->update ();
    }
    else if (key == Qt::Key_I) {
      State::scene ().printStatistics (false);
    }
    else if (State::hasTool ()) {
      if (key == Qt::Key_Escape || key == Qt::Key_Enter) {
        State::tool    ().close ();
        State::setTool (nullptr);
        this->self->update ();
      }
    }
    else if (key == Qt::Key_Escape) {
      QCoreApplication::instance()->quit();
    }
    else if (key == Qt::Key_Z && mod == Qt::ControlModifier) {
      State::history ().undo ();
      this->self->update ();
    }
    else if (key == Qt::Key_Y && mod == Qt::ControlModifier) {
      State::history ().redo ();
      this->self->update ();
    }
    else {
      this->self->QGLWidget::keyPressEvent (e);
    }
  }

  void mouseMoveEvent (QMouseEvent* e) {
    this->self->setFocus (Qt::MouseFocusReason);

    if (e->buttons ().testFlag (Qt::MiddleButton)) {
      this->toolMoveCamera.mouseMoveEvent (*e);
    }
    else if (State::hasTool ()) {
      State::handleToolResponse (State::tool ().mouseMoveEvent (*e));
    }
  }

  void mousePressEvent (QMouseEvent* e) {
    if (e->button () == Qt::MiddleButton) {
      this->toolMoveCamera.mousePressEvent (*e);
    }
    else if (State::hasTool ()) {
      State::handleToolResponse (State::tool ().mousePressEvent (*e));
    }
  }

  void mouseReleaseEvent (QMouseEvent* e) {
    if (e->button () == Qt::MiddleButton) {
      this->toolMoveCamera.mouseReleaseEvent (*e);
    }
    else if (State::hasTool ()) {
      State::handleToolResponse (State::tool ().mouseReleaseEvent (*e));
    }
    else if (e->button () == Qt::LeftButton) {
      this->selectIntersection (ViewUtil::toIVec2 (*e));
    }
    else if (e->button () == Qt::RightButton) {
      glm::ivec2 pos = ViewUtil::toIVec2 (*e);

      if (State::scene ().numSelections () == 0) {
        this->selectIntersection (pos);
      }
      if (State::scene ().numSelections () == 0) {
        ViewMenuNoSelection menu (pos);
        menu.exec (e->globalPos ());
      }
      else {
        switch (State::scene ().selectionMode ()) {
          case SelectionMode::WingedMesh: {
            ViewMenuWingedMesh menu (pos);
            menu.exec (e->globalPos ());
            break;
          }
        }
      }
    }
  }

  void wheelEvent (QWheelEvent* e) {
    this->toolMoveCamera.wheelEvent (*e);
  }
};

DELEGATE_CONSTRUCTOR_BASE ( ViewGlWidget, (const QGLFormat& f, ViewMainWindow& w)
                          , (this,w), QGLWidget, (f))
DELEGATE_DESTRUCTOR (ViewGlWidget)

DELEGATE  (glm::ivec2, ViewGlWidget, cursorPosition)
DELEGATE  (void      , ViewGlWidget, selectIntersection)
DELEGATE1 (void      , ViewGlWidget, selectIntersection, const glm::ivec2&)
DELEGATE  (void      , ViewGlWidget, initializeGL)
DELEGATE2 (void      , ViewGlWidget, resizeGL         , int, int)
DELEGATE1 (void      , ViewGlWidget, paintEvent,QPaintEvent*)
DELEGATE1 (void      , ViewGlWidget, keyPressEvent    , QKeyEvent*)
DELEGATE1 (void      , ViewGlWidget, mouseMoveEvent   , QMouseEvent*)
DELEGATE1 (void      , ViewGlWidget, mousePressEvent  , QMouseEvent*)
DELEGATE1 (void      , ViewGlWidget, mouseReleaseEvent, QMouseEvent*)
DELEGATE1 (void      , ViewGlWidget, wheelEvent       , QWheelEvent*)
