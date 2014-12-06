#include <QApplication>
#include <QKeyEvent>
#include <glm/glm.hpp>
#include <memory>
#include "camera.hpp"
#include "history.hpp"
#include "mesh-type.hpp"
#include "primitive/ray.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "selection-mode.hpp"
#include "state.hpp"
#include "tool.hpp"
#include "tool/move-camera.hpp"
#include "view/axis.hpp"
#include "view/freeform-mesh-menu.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/properties/selection.hpp"
#include "view/properties/widget.hpp"
#include "view/tool-menu-parameters.hpp"
#include "view/util.hpp"

struct ViewGlWidget::Impl {
  ViewGlWidget*                    self;
  ViewMainWindow&                  mainWindow;
  ViewAxis                         axis;
  std::unique_ptr <ToolMoveCamera> toolMoveCamera;

  Impl (ViewGlWidget* s, ViewMainWindow& mW) 
    : self (s)
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

  void setToolMoveCamera (ToolMoveCamera& tool) {
    assert (this->toolMoveCamera == false);
    this->toolMoveCamera.reset (&tool);
    this->handleCameraResponse (this->toolMoveCamera->initialize ());
    this->self->update ();
  }

  void deleteToolMoveCamera () {
    assert (this->toolMoveCamera);
    this->toolMoveCamera->close ();
    this->toolMoveCamera.reset (nullptr);
    this->self->update ();
  }

  void handleCameraResponse (ToolResponse response) {
    switch (response) {
      case ToolResponse::None:
        break;
      case ToolResponse::Redraw:
        this->self->update ();
        break;
      case ToolResponse::Terminate:
        this->deleteToolMoveCamera ();
        break;
    }
  }

  void initializeGL () {
    Renderer::initialize ();
    State   ::initialize ();

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

    State::scene ().render (MeshType::Freeform);

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
    if (this->toolMoveCamera) {
      switch (e->key()) {
        case Qt::Key_Escape:
          this->deleteToolMoveCamera ();
          break;
        default:
          this->self->QGLWidget::keyPressEvent (e);
      }
    }
    else if (State::hasTool ()) {
      switch (e->key()) {
        case Qt::Key_Escape:
          State::tool    ().close ();
          State::setTool (nullptr);
          this->self->update ();
          break;
        default:
          this->self->QGLWidget::keyPressEvent (e);
      }
    }
    else {
      switch (e->key()) {
        case Qt::Key_Escape:
          QCoreApplication::instance()->quit();
          break;
        case Qt::Key_W:
          State :: scene ().toggleRenderMode (MeshType::Freeform);
          this->self->update ();
          break;
        case Qt::Key_I:
          State::scene ().printStatistics (true);
          break;
        case Qt::Key_Z:
          if (e->modifiers () == Qt::ControlModifier) {
            State::history ().undo ();
            this->self->update ();
          }
          else
            this->self->QGLWidget::keyPressEvent (e);
          break;
        case Qt::Key_Y:
          if (e->modifiers () == Qt::ControlModifier) {
            State::history ().redo ();
            this->self->update ();
          }
          else
            this->self->QGLWidget::keyPressEvent (e);
          break;
        default:
          this->self->QGLWidget::keyPressEvent (e);
      }
    }
  }

  void mouseMoveEvent (QMouseEvent* e) {
    if (this->toolMoveCamera) {
      this->handleCameraResponse (this->toolMoveCamera->mouseMoveEvent (*e));
    }
    else if (State::hasTool ()) {
      State::handleToolResponse (State::tool ().mouseMoveEvent (*e));
    }
  }

  void mouseReleaseEvent (QMouseEvent* e) {
    if (e->button () == Qt::MiddleButton && this->toolMoveCamera == false) {
      ViewToolMenuParameters parameters (this->mainWindow, ViewUtil::toIVec2 (*e));
      this->setToolMoveCamera (*new ToolMoveCamera 
          ( parameters
          , e->modifiers ().testFlag (Qt::ShiftModifier)
          ));
    }
    else if (this->toolMoveCamera) {
      this->handleCameraResponse (this->toolMoveCamera->mouseReleaseEvent (*e));
    }
    else if (State::hasTool ()) {
      State::handleToolResponse (State::tool ().mouseReleaseEvent (*e));
    }
    else if (e->button () == Qt::LeftButton) {
      this->selectIntersection (ViewUtil::toIVec2 (*e));
    }
    else if (e->button () == Qt::RightButton) {
      glm::ivec2 pos = ViewUtil::toIVec2 (*e);

      switch (State::scene ().selectionMode ()) {
        case SelectionMode::Freeform: {
          ViewFreeformMeshMenu menu (this->mainWindow, pos);
          menu.exec (e->globalPos ());
          break;
        }
        default:
          std::abort ();
      }
    }
  }

  void wheelEvent (QWheelEvent* e) {
    this->handleCameraResponse (ToolMoveCamera::staticWheelEvent (*e));
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
DELEGATE1 (void      , ViewGlWidget, mouseReleaseEvent, QMouseEvent*)
DELEGATE1 (void      , ViewGlWidget, wheelEvent       , QWheelEvent*)
