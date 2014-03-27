#include <QApplication>
#include <QKeyEvent>
#include <glm/glm.hpp>
#include "view/gl-widget.hpp"
#include "renderer.hpp"
#include "view/mouse-movement.hpp"
#include "state.hpp"
#include "camera.hpp"
#include "tool.hpp"
#include "tool/rotate.hpp"
#include "history.hpp"
#include "scene.hpp"
#include "axis.hpp"
#include "mesh-type.hpp"
#include "view/main-window.hpp"
#include "view/freeform-mesh-menu.hpp"
#include "view/properties/widget.hpp"
#include "view/properties/selection.hpp"

struct ViewGlWidget :: Impl {
  ViewGlWidget*         self;
  ViewMainWindow*       mainWindow;
  Axis                  axis;
  bool                  hasActiveContextMenu;

  Impl (ViewGlWidget* s, ViewMainWindow* mW) 
    : self (s)
    , mainWindow (mW) 
    , hasActiveContextMenu (false)
  {}

  ~Impl () {
    State::setTool (nullptr);
  }

  void initializeGL () {
    Renderer :: initialize ();
    State    :: initialize ();

    this->axis.initialize        ();
    this->self->setMouseTracking (true);
    this->self->setFocus         ();

    Renderer :: updateLights (State :: camera ());

    QObject::connect ( this->mainWindow->properties ()->selection ()
                     , &ViewPropertiesSelection::selectionChanged 
                     , [this] () { this->self->update (); });
    QObject::connect ( this->mainWindow->properties ()->selection ()
                     , &ViewPropertiesSelection::hideOthersChanged
                     , [this] () { this->self->update (); });
  }

  void paintGL () {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (this->mainWindow->properties ()->selection ()->show (MeshType::Freeform)) {
      State :: scene ().render (MeshType::Freeform);
    }
    if (State::hasTool ()) {
      State::tool ().render ();
    }
    this->axis.render ();
  }

  void resizeGL (int w, int h) {
    State :: camera ().updateResolution (glm::uvec2 (w,h));
    glViewport (0,0,w, h < 1 ? 1 : h );
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
  }

  void mouseMoveEvent (QMouseEvent* e) {
    State :: mouseMovement ().update (e->pos ());
    if (e->buttons () == Qt :: MiddleButton) {
      ToolRotate :: run ();
      this->self->update ();
    }
    else if (State::hasTool ()) {
      State::tool ().mouseMoveEvent (e);
    }
  }

  void mousePressEvent (QMouseEvent* e) {
    if (this->hasActiveContextMenu == false
     && e->buttons () == Qt :: LeftButton
     && State::hasTool () ) {

      State::tool ().mousePressEvent (e);
    }
  }

  void mouseReleaseEvent (QMouseEvent* e) {
    State :: mouseMovement ().invalidate ();
    if (State::hasTool ()) {
      State::tool ().mouseReleaseEvent (e);
    }
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

  void contextMenuEvent (QContextMenuEvent* e) {
    State::setTool (nullptr);
    if (this->mainWindow->properties ()->selection ()->selected (MeshType::Freeform)) {
      ViewFreeformMeshMenu menu (this->mainWindow, e);
      this->hasActiveContextMenu = true;
      menu.exec (QCursor::pos ());
      this->mainWindow->activateWindow ();
    }
    this->hasActiveContextMenu = false;
  }
};

ViewGlWidget :: ViewGlWidget (const QGLFormat& format, ViewMainWindow* mainWindow) 
  : QGLWidget (format) 
{ 
  this->impl = new Impl (this, mainWindow);
}

DELEGATE_BIG3_WITHOUT_CONSTRUCTOR (ViewGlWidget)

DELEGATE  (void, ViewGlWidget, initializeGL)
DELEGATE2 (void, ViewGlWidget, resizeGL         , int, int)
DELEGATE  (void, ViewGlWidget, paintGL)
DELEGATE1 (void, ViewGlWidget, keyPressEvent    , QKeyEvent*)
DELEGATE1 (void, ViewGlWidget, mouseMoveEvent   , QMouseEvent*)
DELEGATE1 (void, ViewGlWidget, mousePressEvent  , QMouseEvent*)
DELEGATE1 (void, ViewGlWidget, mouseReleaseEvent, QMouseEvent*)
DELEGATE1 (void, ViewGlWidget, resizeEvent      , QResizeEvent*)
DELEGATE1 (void, ViewGlWidget, wheelEvent       , QWheelEvent*)
DELEGATE1 (void, ViewGlWidget, contextMenuEvent , QContextMenuEvent*)
