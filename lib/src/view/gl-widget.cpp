/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCoreApplication>
#include <QMouseEvent>
#include <QPainter>
#include <glm/glm.hpp>
#include "camera.hpp"
#include "config.hpp"
#include "mesh-util.hpp"
#include "mesh.hpp"
#include "opengl.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool/move-camera.hpp"
#include "view/axis.hpp"
#include "view/floor-plane.hpp"
#include "view/gl-widget.hpp"
#include "view/info-pane.hpp"
#include "view/info-pane/scene.hpp"
#include "view/key-event.hpp"
#include "view/main-window.hpp"
#include "view/pointing-event.hpp"
#include "view/tool-pane.hpp"
#include "view/util.hpp"

struct ViewGlWidget::Impl
{
  typedef std::unique_ptr<ToolMoveCamera> ToolMoveCameraPtr;
  typedef std::unique_ptr<State>          StatePtr;
  typedef std::unique_ptr<ViewAxis>       AxisPtr;
  typedef std::unique_ptr<ViewFloorPlane> FloorPlanePtr;

  ViewGlWidget*     self;
  ViewMainWindow&   mainWindow;
  Config&           config;
  Cache&            cache;
  ToolMoveCameraPtr _immediateMoveCamera;
  StatePtr          _state;
  AxisPtr           axis;
  FloorPlanePtr     _floorPlane;
  bool              tabletPressed;

  Impl (ViewGlWidget* s, ViewMainWindow& mW, Config& cfg, Cache& cch)
    : self (s)
    , mainWindow (mW)
    , config (cfg)
    , cache (cch)
    , tabletPressed (false)
  {
    this->self->setAutoFillBackground (false);
  }

  ~Impl ()
  {
    this->self->makeCurrent ();

    this->_state.reset (nullptr);
    this->axis.reset (nullptr);
    this->_floorPlane.reset (nullptr);

    this->self->doneCurrent ();
  }

  ToolMoveCamera& immediateMoveCamera ()
  {
    assert (this->_immediateMoveCamera);
    return *this->_immediateMoveCamera;
  }

  State& state ()
  {
    assert (this->_state);
    return *this->_state;
  }

  ViewFloorPlane& floorPlane ()
  {
    assert (this->_floorPlane);
    return *this->_floorPlane;
  }

  glm::ivec2 cursorPosition ()
  {
    return ViewUtil::toIVec2 (this->self->mapFromGlobal (QCursor::pos ()));
  }

  void fromConfig ()
  {
    assert (this->axis);
    assert (this->_immediateMoveCamera);

    this->state ().fromConfig ();
    this->axis->fromConfig (this->config);

    this->floorPlane ().fromConfig (this->config);
    this->floorPlane ().update (this->state ().camera ());

    this->_immediateMoveCamera->fromConfig ();
  }

  void initializeGL ()
  {
    OpenGL::initializeFunctions (this->config.get<bool> ("editor/use-geometry-shader"));

    this->_state.reset (new State (this->mainWindow, this->config, this->cache));
    this->axis.reset (new ViewAxis (this->config));
    this->_floorPlane.reset (new ViewFloorPlane (this->config, this->state ().camera ()));
    this->_immediateMoveCamera.reset (new ToolMoveCamera (this->state (), true));
    this->_immediateMoveCamera->initialize ();

    this->self->setMouseTracking (true);
    this->self->setTabletTracking (true);
    this->initializeScene ();
    this->mainWindow.toolPane ().forceWidth ();
  }

  void initializeScene ()
  {
    const QStringList arguments = QCoreApplication::arguments ();
    if (arguments.size () > 1)
    {
      const std::string fileName = arguments.at (1).toStdString ();
      if (this->state ().scene ().fromDlyFile (this->state ().config (), fileName) == false)
      {
        ViewUtil::error (this->mainWindow, QObject::tr ("Could not open file."));
      }
    }
    else
    {
      this->state ().scene ().newDynamicMesh (this->config, MeshUtil::icosphere (4));
    }
    this->mainWindow.update ();
  }

  void paintGL ()
  {
    QPainter painter (this->self);
    painter.beginNativePainting ();

    this->state ().camera ().renderer ().setupRendering ();
    this->state ().scene ().render (this->state ().camera ());
    this->floorPlane ().render (this->state ().camera ());

    if (this->state ().hasTool ())
    {
      this->state ().tool ().render ();
    }
    this->axis->render (this->state ().camera ());

    this->state ().camera ().renderer ().shutdownRendering ();
    painter.endNativePainting ();

    this->axis->render (this->state ().camera (), painter);
    if (this->state ().hasTool ())
    {
      this->state ().tool ().paint (painter);
    }
  }

  void resizeGL (int w, int h) { this->state ().camera ().updateResolution (glm::uvec2 (w, h)); }

  void pointingEvent (const ViewPointingEvent& e)
  {
    if (e.valid ())
    {
      if (this->_immediateMoveCamera->pointingEvent (e) == ToolResponse::Redraw)
      {
        this->state ().handleToolResponse (ToolResponse::Redraw);
        this->updateCursorInTool ();
      }

      if (this->state ().hasTool ())
      {
        this->state ().handleToolResponse (this->state ().tool ().pointingEvent (e));
      }
    }
  }

  void mouseMoveEvent (QMouseEvent* e)
  {
    if (this->tabletPressed == false)
    {
      this->pointingEvent (ViewPointingEvent (*e));
    }
  }

  void mousePressEvent (QMouseEvent* e)
  {
    if (this->tabletPressed == false)
    {
      this->pointingEvent (ViewPointingEvent (*e));
    }
  }

  void mouseReleaseEvent (QMouseEvent* e)
  {
    if (this->tabletPressed == false)
    {
      this->pointingEvent (ViewPointingEvent (*e));
    }
  }

  void wheelEvent (QWheelEvent* e)
  {
    if (this->_immediateMoveCamera->wheelEvent (*e) == ToolResponse::Redraw)
    {
      this->state ().handleToolResponse (ToolResponse::Redraw);
      this->updateCursorInTool ();
    }
  }

  void tabletEvent (QTabletEvent* e)
  {
    const ViewPointingEvent pointingEvent (this->state ().config (), *e);

    if (pointingEvent.pressEvent ())
    {
      this->tabletPressed = true;
    }
    else if (pointingEvent.releaseEvent ())
    {
      this->tabletPressed = false;
    }
    this->pointingEvent (pointingEvent);
  }

  void keyPressEvent (QKeyEvent* e)
  {
    const ViewKeyEvent keyEvent (*e, true);

    if (this->state ().hasTool ())
    {
      this->state ().tool ().keyEvent (keyEvent);
    }
    this->self->QOpenGLWidget::keyPressEvent (e);
  }

  void keyReleaseEvent (QKeyEvent* e)
  {
    const ViewKeyEvent keyEvent (*e, false);

    if (this->state ().hasTool ())
    {
      this->state ().tool ().keyEvent (keyEvent);
    }
    this->self->QOpenGLWidget::keyReleaseEvent (e);
  }

  void enterEvent (QEvent*) { this->self->setFocus (); }

  void updateCursorInTool ()
  {
    if (this->state ().hasTool ())
    {
      this->state ().handleToolResponse (
        this->state ().tool ().cursorUpdate (this->cursorPosition ()));
    }
  }
};

DELEGATE3_BIG2_SELF (ViewGlWidget, ViewMainWindow&, Config&, Cache&)
DELEGATE (ToolMoveCamera&, ViewGlWidget, immediateMoveCamera)
DELEGATE (State&, ViewGlWidget, state)
DELEGATE (ViewFloorPlane&, ViewGlWidget, floorPlane)
DELEGATE (glm::ivec2, ViewGlWidget, cursorPosition)
DELEGATE (void, ViewGlWidget, fromConfig)
DELEGATE (void, ViewGlWidget, initializeGL)
DELEGATE2 (void, ViewGlWidget, resizeGL, int, int)
DELEGATE (void, ViewGlWidget, paintGL)
DELEGATE1 (void, ViewGlWidget, mouseMoveEvent, QMouseEvent*)
DELEGATE1 (void, ViewGlWidget, mousePressEvent, QMouseEvent*)
DELEGATE1 (void, ViewGlWidget, mouseReleaseEvent, QMouseEvent*)
DELEGATE1 (void, ViewGlWidget, wheelEvent, QWheelEvent*)
DELEGATE1 (void, ViewGlWidget, tabletEvent, QTabletEvent*)
DELEGATE1 (void, ViewGlWidget, keyPressEvent, QKeyEvent*)
DELEGATE1 (void, ViewGlWidget, keyReleaseEvent, QKeyEvent*)
DELEGATE1 (void, ViewGlWidget, enterEvent, QEvent*)
