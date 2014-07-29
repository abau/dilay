#include <memory>
#include <QApplication>
#include <QKeyEvent>
#include <glm/glm.hpp>
#include "view/main-window.hpp"
#include "view/freeform-mesh-menu.hpp"
#include "view/sphere-mesh-menu.hpp"
#include "view/properties/widget.hpp"
#include "view/properties/selection.hpp"
#include "view/gl-widget.hpp"
#include "view/util.hpp"
#include "view/tool-menu-parameters.hpp"
#include "renderer.hpp"
#include "state.hpp"
#include "camera.hpp"
#include "tool.hpp"
#include "tool/move-camera.hpp"
#include "history.hpp"
#include "scene.hpp"
#include "axis.hpp"
#include "mesh-type.hpp"
#include "primitive/ray.hpp"
#include "selection-mode.hpp"

typedef std::unique_ptr <ToolMoveCamera> MoveCamera;

struct ViewGlWidget::Impl {
  ViewGlWidget*   self;
  ViewMainWindow& mainWindow;
  Axis            axis;
  MoveCamera      toolMoveCamera;

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

  void handleCameraResponse (ToolResponse response) {
    switch (response) {
      case ToolResponse::None:
        break;
      case ToolResponse::Redraw:
        this->self->update ();
        break;
      case ToolResponse::Terminate:
        this->self->update ();
        this->toolMoveCamera.reset ();
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
    State::scene ().render (MeshType::Sphere);

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
    if (State::hasTool ()) {
      switch (e->key()) {
        case Qt::Key_Escape:
          State::setTool (nullptr);
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
      ViewToolMenuParameters parameters (this->mainWindow, ViewUtil::toIVec2 (*e),false);
      this->toolMoveCamera.reset (new ToolMoveCamera ( parameters
                                                     , e->modifiers ().testFlag (Qt::ShiftModifier)
                                                     ));
      this->handleCameraResponse (this->toolMoveCamera->initialize ());
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
          menu.exec (QCursor::pos ());
          break;
        }
        case SelectionMode::Sphere: {
          ViewSphereMeshMenu menu (this->mainWindow, pos);
          menu.exec (QCursor::pos ());
          break;
        }
        default:
          assert (false);
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
