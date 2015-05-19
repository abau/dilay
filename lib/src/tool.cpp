#include <QMouseEvent>
#include <glm/glm.hpp>
#include "cache.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "history.hpp"
#include "octree.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool.hpp"
#include "view/gl-widget.hpp"
#include "view/main-window.hpp"
#include "view/properties.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"

struct Tool::Impl {
  Tool*       self;
  State&      state;
  Config&     config;
  CacheProxy _cache;

  Impl (Tool* s, State& st, const char* key) 
    : self           (s) 
    , state          (st)
    , config         (this->state.config ())
    ,_cache          (this->cache (key))
  {
    this->state.mainWindow ().showToolTip (ViewToolTip ());
  }

  ToolResponse initialize () { 
    return this->self->runInitialize ();
  }

  void render () const { 
    return this->self->runRender ( ); 
  }

  ToolResponse mouseMoveEvent (const QMouseEvent& e) { 
    return this->self->runMouseMoveEvent (e);
  }

  ToolResponse mousePressEvent (const QMouseEvent& e) {
    return this->self->runMousePressEvent (e);
  }

  ToolResponse mouseReleaseEvent (const QMouseEvent& e) {
    return this->self->runMouseReleaseEvent (e);
  }

  ToolResponse wheelEvent (const QWheelEvent& e) {
    return this->self->runWheelEvent (e);
  }

  void close () { 
    return this->self->runClose (); 
  }

  void updateGlWidget () {
    this->state.mainWindow ().glWidget ().update ();
  }

  ViewProperties& properties () const {
    return this->state.mainWindow ().properties ();
  }

  void showToolTip (const ViewToolTip& toolTip) {
    this->state.mainWindow ().showToolTip (toolTip);
  }

  CacheProxy& cache () {
    return this->_cache;
  }

  CacheProxy cache (const char* key) const {
    return CacheProxy (this->state.cache (), "editor/tool/" + std::string (key) + "/");
  }

  glm::ivec2 cursorPosition () {
    return this->state.mainWindow ().glWidget ().cursorPosition ();
  }

  bool intersectsScene (const glm::ivec2& pos, WingedFaceIntersection& intersection) {
    return this->state.scene ().intersects ( this->state.camera ().ray (pos)
                                           , intersection );
  }

  bool intersectsScene (const QMouseEvent& e, WingedFaceIntersection& intersection) {
    return this->intersectsScene (ViewUtil::toIVec2 (e), intersection);
  }

  bool intersectsRecentOctree (const QMouseEvent& e, Intersection& intersection) const {
    if (this->state.history ().hasRecentOctrees ()) {
      const PrimRay ray = this->state.camera ().ray (ViewUtil::toIVec2 (e));
      OctreeIntersection octreeIntersection;

      this->state.history ().forEachRecentOctree (
        [&ray, &octreeIntersection] (const Mesh& mesh, const Octree& octree) {
          octree.intersects (mesh, ray, octreeIntersection);
        }
      );
      intersection = octreeIntersection;
      return intersection.isIntersection ();
    }
    else {
      return false;
    }
  }

  void snapshotScene () {
    this->state.history ().snapshot (this->state.scene ());
  }
};

DELEGATE2_BIG3_SELF (Tool, State&, const char*)
DELEGATE        (ToolResponse   , Tool, initialize)
DELEGATE_CONST  (void           , Tool, render)
DELEGATE1       (ToolResponse   , Tool, mouseMoveEvent, const QMouseEvent&)
DELEGATE1       (ToolResponse   , Tool, mousePressEvent, const QMouseEvent&)
DELEGATE1       (ToolResponse   , Tool, mouseReleaseEvent, const QMouseEvent&)
DELEGATE1       (ToolResponse   , Tool, wheelEvent, const QWheelEvent&)
DELEGATE        (void           , Tool, close)
GETTER_CONST    (State&         , Tool, state)
DELEGATE        (void           , Tool, updateGlWidget)
DELEGATE_CONST  (ViewProperties&, Tool, properties)
DELEGATE1       (void           , Tool, showToolTip, const ViewToolTip&)
GETTER_CONST    (Config&        , Tool, config)
DELEGATE        (CacheProxy&    , Tool, cache)
DELEGATE1_CONST (CacheProxy     , Tool, cache, const char*)
DELEGATE_CONST  (glm::ivec2     , Tool, cursorPosition)
DELEGATE2       (bool           , Tool, intersectsScene, const glm::ivec2&, WingedFaceIntersection&)
DELEGATE2       (bool           , Tool, intersectsScene, const QMouseEvent&, WingedFaceIntersection&)
DELEGATE2_CONST (bool           , Tool, intersectsRecentOctree, const QMouseEvent&, Intersection&)
DELEGATE        (void           , Tool, snapshotScene)
