/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "cache.hpp"
#include "camera.hpp"
#include "history.hpp"
#include "octree.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool.hpp"
#include "view/gl-widget.hpp"
#include "view/main-widget.hpp"
#include "view/main-window.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"

struct Tool::Impl {
  Tool*       self;
  State&      state;
  CacheProxy _cache;

  Impl (Tool* s, State& st, const char* key) 
    : self  (s) 
    , state (st)
    ,_cache (this->cache (key))
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
    this->state.mainWindow ().mainWidget ().glWidget ().update ();
  }

  ViewProperties& properties () const {
    return this->state.mainWindow ().mainWidget ().properties ();
  }

  void showToolTip (const ViewToolTip& toolTip) {
    this->state.mainWindow ().showToolTip (toolTip);
  }

  Config& config () const {
    return this->state.config ();
  }

  CacheProxy& cache () {
    return this->_cache;
  }

  CacheProxy cache (const char* key) const {
    return CacheProxy (this->state.cache (), "editor/tool/" + std::string (key) + "/");
  }

  glm::ivec2 cursorPosition () {
    return this->state.mainWindow ().mainWidget ().glWidget ().cursorPosition ();
  }

  void snapshotWingedMeshes () {
    this->state.history ().snapshotWingedMeshes (this->state.scene ());
  }

  void snapshotSketchMeshes () {
    this->state.history ().snapshotSketchMeshes (this->state.scene ());
  }

  bool intersectsRecentOctree (const QMouseEvent& e, Intersection& intersection) const {
    assert (this->state.history ().hasRecentOctrees ());

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

  template <typename T>
  bool intersectsScene (const glm::ivec2& pos, T& intersection) {
    return this->state.scene ().intersects (this->state.camera ().ray (pos), intersection);
  }

  template <typename T>
  bool intersectsScene (const QMouseEvent& e, T& intersection) {
    return this->intersectsScene (ViewUtil::toIVec2 (e), intersection);
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
DELEGATE_CONST  (Config&        , Tool, config)
DELEGATE        (CacheProxy&    , Tool, cache)
DELEGATE1_CONST (CacheProxy     , Tool, cache, const char*)
DELEGATE_CONST  (glm::ivec2     , Tool, cursorPosition)
DELEGATE        (void           , Tool, snapshotWingedMeshes)
DELEGATE        (void           , Tool, snapshotSketchMeshes)
DELEGATE2_CONST (bool           , Tool, intersectsRecentOctree, const QMouseEvent&, Intersection&)

template <typename T>
bool Tool :: intersectsScene (const glm::ivec2& pos, T& intersection) {
  return this->impl->intersectsScene (pos, intersection);
}

template <typename T>
bool Tool :: intersectsScene (const QMouseEvent& e, T& intersection) {
  return this->impl->intersectsScene (e, intersection);
}

template bool Tool :: intersectsScene (const glm::ivec2&, WingedFaceIntersection&);
template bool Tool :: intersectsScene (const QMouseEvent&, WingedFaceIntersection&);
template bool Tool :: intersectsScene (const glm::ivec2&, SketchNodeIntersection&);
template bool Tool :: intersectsScene (const QMouseEvent&, SketchNodeIntersection&);
