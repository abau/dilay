#include "tool.hpp"
#include "view/tool-options.hpp"
#include "view/main-window.hpp"
#include "view/gl-widget.hpp"
#include "state.hpp"
#include "scene.hpp"

struct Tool::Impl {
  Tool*              self;
  ViewMainWindow&    mainWindow;
  QContextMenuEvent& menuEvent;
  ViewToolOptions&   toolOptions;
  bool               isDraged;
  bool               isHovered;

  Impl (Tool* s, ViewMainWindow& w, QContextMenuEvent& e, const QString& name) 
    : self        (s) 
    , mainWindow  (w)
    , menuEvent   (e)
    , toolOptions (*new ViewToolOptions (w))
    , isDraged    (false)
    , isHovered   (false)
  {
    this->toolOptions.setWindowTitle (name);

    QObject::connect ( &this->toolOptions, &ViewToolOptions::accepted
                     , [this] () { State::setTool (nullptr); } );

    QObject::connect ( &this->toolOptions, &ViewToolOptions::rejected
                     , [this] () { State::setTool (nullptr); } );

    this->toolOptions.show ();
  }

  ~Impl () {
    this->drag               (false);
    this->hover              (false);
    this->selectIntersection ();
    this->toolOptions.close  ();
  }

  void render () { 
    return this->self->runRender ( ); 
  }

  void mouseMoveEvent (QMouseEvent& e) { 
    if (this->self->runMouseMoveEvent (e)) {
      this->updateGlWidget ();
    }
  }

  void mousePressEvent (QMouseEvent& e) {
    if (this->self->runMousePressEvent (e)) {
      this->updateGlWidget ();
    }
  }

  void mouseReleaseEvent (QMouseEvent& e) {
    if (this->self->runMouseReleaseEvent (e)) {
      this->updateGlWidget ();
    }
  }

  void wheelEvent (QWheelEvent& e) {
    if (this->self->runWheelEvent (e)) {
      this->updateGlWidget ();
    }
  }

  void updateGlWidget () {
    this->mainWindow.glWidget ().update ();
  }

  void unselectAll () {
    if (State::scene ().unselectAll ()) {
      this->updateGlWidget ();
    }
  }

  void selectIntersection () {
    this->mainWindow.glWidget ().selectIntersection ();
  }

  void selectIntersection (const glm::ivec2& pos) {
    this->mainWindow.glWidget ().selectIntersection (pos);
  }

  void hoverIntersection () {
    this->mainWindow.glWidget ().hoverIntersection ();
  }

  void hoverIntersection (const glm::ivec2& pos) {
    this->mainWindow.glWidget ().hoverIntersection (pos);
  }

  void drag (bool b) { 
    this->isDraged = b;

    if (this->isDraged) {
      assert (this->isHovered);
      this->mainWindow.glWidget ().setCursor (QCursor (Qt::ClosedHandCursor));
    }
    else {
      this->mainWindow.glWidget ().unsetCursor ();
    }
  }

  void dragIfHovered () {
    if (this->isHovered) {
      this->drag (true);
    }
  }

  void hover (bool b) { 
    assert (! this->isDraged);

    this->isHovered = b;

    if (this->isHovered) {
      this->mainWindow.glWidget ().setCursor (QCursor (Qt::OpenHandCursor));
    }
    else {
      this->mainWindow.glWidget ().unsetCursor ();
    }
  }

  void hoverIfDraged () {
    if (this->isDraged) {
      this->drag  (false);
      this->hover (true);
    }
  }
};

DELEGATE3_BIG3_SELF  (Tool, ViewMainWindow&, QContextMenuEvent&, const QString&)
GETTER         (ViewMainWindow&   , Tool, mainWindow)
GETTER         (QContextMenuEvent&, Tool, menuEvent)
DELEGATE       (void              , Tool, updateGlWidget)
DELEGATE       (void              , Tool, unselectAll)
DELEGATE       (void              , Tool, selectIntersection)
DELEGATE1      (void              , Tool, selectIntersection, const glm::ivec2&)
DELEGATE       (void              , Tool, hoverIntersection)
DELEGATE1      (void              , Tool, hoverIntersection, const glm::ivec2&)
DELEGATE       (void              , Tool, render)
DELEGATE1      (void              , Tool, mouseMoveEvent, QMouseEvent&)
DELEGATE1      (void              , Tool, mousePressEvent, QMouseEvent&)
DELEGATE1      (void              , Tool, mouseReleaseEvent, QMouseEvent&)
DELEGATE1      (void              , Tool, wheelEvent, QWheelEvent&)
GETTER_CONST   (bool              , Tool, isDraged)
GETTER_CONST   (bool              , Tool, isHovered)
GETTER         (ViewToolOptions&  , Tool, toolOptions)
DELEGATE1      (void              , Tool, drag, bool)
DELEGATE       (void              , Tool, dragIfHovered)
DELEGATE1      (void              , Tool, hover, bool)
DELEGATE       (void              , Tool, hoverIfDraged)
