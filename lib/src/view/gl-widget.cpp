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
#include "view/mouse-movement.hpp"
#include "view/util.hpp"
#include "renderer.hpp"
#include "state.hpp"
#include "camera.hpp"
#include "tool.hpp"
#include "tool/rotate.hpp"
#include "history.hpp"
#include "scene.hpp"
#include "axis.hpp"
#include "mesh-type.hpp"
#include "primitive/ray.hpp"

struct ViewGlWidget::Impl {
  ViewGlWidget*                self;
  ViewMainWindow&              mainWindow;
  Axis                         axis;
  std::unique_ptr <ToolRotate> toolRotate;

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
    if (State::scene ().selectIntersection 
          ( this->mainWindow.properties ().selection ().selected ()
          , State::camera ().ray (pos))) 
    {
      this->self->update ();
    }
  }

  void invalidateToolRotate () {
    this->toolRotate.reset ();
  }

  void initializeGL () {
    Renderer::initialize ();
    State   ::initialize ();

    this->axis.initialize        ();
    this->self->setMouseTracking (true);
    this->self->setFocus         ();

    Renderer::updateLights (State::camera ());

    QObject::connect ( &this->mainWindow.properties ().selection ()
                     , &ViewPropertiesSelection::selectionChanged 
                     , [this] () { this->self->update (); });
    QObject::connect ( &this->mainWindow.properties ().selection ()
                     , &ViewPropertiesSelection::hideOthersChanged
                     , [this] () { this->self->update (); });
  }

  void paintEvent (QPaintEvent*) {
    this->self->makeCurrent ();

    Renderer::renderInitialize ();

    if (this->mainWindow.properties ().selection ().show (MeshType::Freeform)) {
      State::scene ().render (MeshType::Freeform);
    }
    if (this->mainWindow.properties ().selection ().show (MeshType::Sphere)) {
      State::scene ().render (MeshType::Sphere);
    }
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
    State::mouseMovement ().update (e->pos ());
    
    if (this->toolRotate) {
      this->toolRotate->mouseMoveEvent (*e);
    }
    else if (State::hasTool ()) {
      State::tool ().mouseMoveEvent (*e);
    }
    else {
      this->selectIntersection (ViewUtil::toIVec2 (*e));
    }
  }

  void mousePressEvent (QMouseEvent* e) {
    if (State::hasTool ()) {
      State::tool ().mousePressEvent (*e);
    }
  }

  void mouseReleaseEvent (QMouseEvent* e) {
    State::mouseMovement ().invalidate ();

    if (e->button () == Qt::MiddleButton && this->toolRotate == false) {
      this->toolRotate.reset (new ToolRotate (this->mainWindow, ViewUtil::toIVec2 (*e)));
    }
    else if (this->toolRotate) {
      this->toolRotate->mouseReleaseEvent (*e);
    }
    else if (State::hasTool ()) {
      State::tool ().mouseReleaseEvent (*e);
    }
  }

  void wheelEvent (QWheelEvent* e) {
    if (this->toolRotate) {
      this->toolRotate->wheelEvent (*e);
    }
    else if (State::hasTool ()) {
      State::tool ().wheelEvent (*e);
    }
  }

  /*
  void contextMenuEvent (QContextMenuEvent* e) {
    State::setTool (nullptr);
    switch (this->mainWindow.properties ().selection ().selected ()) {
      case MeshType::Freeform: {
        ViewFreeformMeshMenu menu (this->mainWindow, *e);
        menu.exec (QCursor::pos ());
        break;
      }
      case MeshType::Sphere: {
        ViewSphereMeshMenu menu (this->mainWindow, *e);
        menu.exec (QCursor::pos ());
        break;
      }
      default:
        assert (false);
    }
    this->mainWindow.activateWindow ();
  }
  */
};

DELEGATE_CONSTRUCTOR_BASE ( ViewGlWidget, (const QGLFormat& f, ViewMainWindow& w)
                          , (this,w), QGLWidget, (f))
DELEGATE_DESTRUCTOR (ViewGlWidget)

DELEGATE  (glm::ivec2, ViewGlWidget, cursorPosition)
DELEGATE  (void      , ViewGlWidget, selectIntersection)
DELEGATE1 (void      , ViewGlWidget, selectIntersection, const glm::ivec2&)
DELEGATE  (void      , ViewGlWidget, invalidateToolRotate)
DELEGATE  (void      , ViewGlWidget, initializeGL)
DELEGATE2 (void      , ViewGlWidget, resizeGL         , int, int)
DELEGATE1 (void      , ViewGlWidget, paintEvent,QPaintEvent*)
DELEGATE1 (void      , ViewGlWidget, keyPressEvent    , QKeyEvent*)
DELEGATE1 (void      , ViewGlWidget, mouseMoveEvent   , QMouseEvent*)
DELEGATE1 (void      , ViewGlWidget, mousePressEvent  , QMouseEvent*)
DELEGATE1 (void      , ViewGlWidget, mouseReleaseEvent, QMouseEvent*)
DELEGATE1 (void      , ViewGlWidget, wheelEvent       , QWheelEvent*)
//DELEGATE1 (void      , ViewGlWidget, contextMenuEvent , QContextMenuEvent*)
