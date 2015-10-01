/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "cache.hpp"
#include "camera.hpp"
#include "dimension.hpp"
#include "history.hpp"
#include "index-octree.hpp"
#include "intersection.hpp"
#include "mesh.hpp"
#include "mirror.hpp"
#include "primitive/ray.hpp"
#include "primitive/triangle.hpp"
#include "scene.hpp"
#include "sketch/mesh.hpp"
#include "state.hpp"
#include "tool.hpp"
#include "view/gl-widget.hpp"
#include "view/main-widget.hpp"
#include "view/main-window.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"
#include "winged/mesh.hpp"

struct Tool::Impl {
  Tool*           self;
  State&          state;
  CacheProxy     _cache;
  Maybe <Mirror> _mirror;
  bool            renderMirror;

  Impl (Tool* s, State& st, const char* key) 
    : self         (s) 
    , state        (st)
    ,_cache        (this->cache (key))
    , renderMirror (true)
  {
    this->state.mainWindow ().showToolTip (ViewToolTip ());
    this->mirror (this->hasMirror ());
  }

  ToolResponse initialize () { 
    return this->self->runInitialize ();
  }

  void render () const { 
    this->self->runRender (); 
    if (this->_mirror && this->renderMirror) {
      this->_mirror->render (this->state.camera ());
    }
  }

  ToolResponse mouseMoveEvent (const QMouseEvent& e) { 
    return this->self->runMouseMoveEvent (e);
  }

  ToolResponse mousePressEvent (const QMouseEvent& e) {
    return this->self->runMousePressEvent (e);
  }

  ToolResponse mouseReleaseEvent (const QMouseEvent& e) {
    ToolResponse response = this->self->runMouseReleaseEvent (e);
    this->state.scene ().sanitizeMeshes ();
    return response;
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

  void snapshotAll () {
    this->state.history ().snapshotAll (this->state.scene ());
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

    this->state.history ().forEachRecentOctree (
      [&ray, &intersection] (const Mesh& mesh, const IndexOctree& octree) {
        octree.intersects (ray, [&ray, &mesh, &intersection] (unsigned int i) {
          const PrimTriangle tri ( mesh.vertex (mesh.index ((3 * i) + 0))
                                 , mesh.vertex (mesh.index ((3 * i) + 1))
                                 , mesh.vertex (mesh.index ((3 * i) + 2)) );
          float t;

          if (IntersectionUtil::intersects (ray, tri, &t)) {
            intersection.update (t, ray.pointAt (t), tri.normal ());
          }
        });
      }
    );
    return intersection.isIntersection ();
  }

  bool hasMirror () const {
    return this->state.cache ().get ("editor/tool/mirror", true);
  }

  const Mirror& mirror () const {
    assert (this->hasMirror ());
    return *this->_mirror;
  }

  void mirror (bool m) {
    this->state.cache ().set ("editor/tool/mirror", m);

    if (m) {
      this->_mirror = Maybe <Mirror>::make (this->config (), Dimension::X);
    }
    else {
      this->_mirror.reset ();
    }
    this->updateGlWidget ();
  }

  const Dimension* mirrorDimension () const {
    const static Dimension d = Dimension::X;

    return this->hasMirror () ? &d : nullptr;
  }

  void mirrorWingedMeshes () {
    assert (this->hasMirror ());

    this->snapshotWingedMeshes ();
    this->state.scene ().forEachMesh (
      [this] (WingedMesh& mesh) {
        mesh.mirror (this->mirror ().plane ());
        mesh.bufferData ();
      }
    );
  }
  void mirrorSketchMeshes () {
    assert (this->hasMirror ());

    this->snapshotSketchMeshes ();
    this->state.scene ().forEachMesh (
      [this] (SketchMesh& mesh) {
        mesh.mirror (*this->mirrorDimension ());
      }
    );
  }

  template <typename T, typename ... Ts>
  bool intersectsScene (const glm::ivec2& pos, T& intersection, Ts ... args) {
    return this->state.scene ().intersects ( this->state.camera ().ray (pos), intersection
                                           , std::forward <Ts> (args) ... );
  }

  template <typename T, typename ... Ts>
  bool intersectsScene (const QMouseEvent& e, T& intersection, Ts ... args) {
    return this->intersectsScene ( ViewUtil::toIVec2 (e), intersection 
                                 , std::forward <Ts> (args) ... );
  }
};

DELEGATE2_BIG3_SELF (Tool, State&, const char*)
DELEGATE        (ToolResponse    , Tool, initialize)
DELEGATE_CONST  (void            , Tool, render)
DELEGATE1       (ToolResponse    , Tool, mouseMoveEvent, const QMouseEvent&)
DELEGATE1       (ToolResponse    , Tool, mousePressEvent, const QMouseEvent&)
DELEGATE1       (ToolResponse    , Tool, mouseReleaseEvent, const QMouseEvent&)
DELEGATE1       (ToolResponse    , Tool, wheelEvent, const QWheelEvent&)
DELEGATE        (void            , Tool, close)
GETTER_CONST    (State&          , Tool, state)
DELEGATE        (void            , Tool, updateGlWidget)
DELEGATE_CONST  (ViewProperties& , Tool, properties)
DELEGATE1       (void            , Tool, showToolTip, const ViewToolTip&)
DELEGATE_CONST  (Config&         , Tool, config)
DELEGATE        (CacheProxy&     , Tool, cache)
DELEGATE1_CONST (CacheProxy      , Tool, cache, const char*)
DELEGATE_CONST  (glm::ivec2      , Tool, cursorPosition)
DELEGATE        (void            , Tool, snapshotAll)
DELEGATE        (void            , Tool, snapshotWingedMeshes)
DELEGATE        (void            , Tool, snapshotSketchMeshes)
DELEGATE2_CONST (bool            , Tool, intersectsRecentOctree, const QMouseEvent&, Intersection&)
DELEGATE_CONST  (bool            , Tool, hasMirror)
DELEGATE_CONST  (const Mirror&   , Tool, mirror)
DELEGATE1       (void            , Tool, mirror, bool)
SETTER          (bool            , Tool, renderMirror)
DELEGATE_CONST  (const Dimension*, Tool, mirrorDimension)
DELEGATE        (void            , Tool, mirrorWingedMeshes)
DELEGATE        (void            , Tool, mirrorSketchMeshes)

template <typename T, typename ... Ts>
bool Tool :: intersectsScene (const glm::ivec2& pos, T& intersection, Ts ... args) {
  return this->impl->intersectsScene (pos, intersection, std::forward <Ts> (args) ...);
}

template <typename T, typename ... Ts>
bool Tool :: intersectsScene (const QMouseEvent& e, T& intersection, Ts ... args) {
  return this->impl->intersectsScene (e, intersection, std::forward <Ts> (args) ...);
}

template bool Tool :: intersectsScene (const glm::ivec2&, WingedFaceIntersection&);
template bool Tool :: intersectsScene (const QMouseEvent&, WingedFaceIntersection&);
template bool Tool :: intersectsScene (const glm::ivec2&, SketchNodeIntersection&);
template bool Tool :: intersectsScene (const QMouseEvent&, SketchNodeIntersection&);
template bool Tool :: intersectsScene (const glm::ivec2&, SketchBoneIntersection&);
template bool Tool :: intersectsScene (const QMouseEvent&, SketchBoneIntersection&);
template bool Tool :: intersectsScene (const glm::ivec2&, SketchMeshIntersection&);
template bool Tool :: intersectsScene (const QMouseEvent&, SketchMeshIntersection&);
template bool Tool :: intersectsScene (const glm::ivec2&, SketchMeshIntersection&, unsigned int);
template bool Tool :: intersectsScene (const QMouseEvent&, SketchMeshIntersection&, unsigned int);
template bool Tool :: intersectsScene (const glm::ivec2&, Intersection&);
template bool Tool :: intersectsScene (const QMouseEvent&, Intersection&);
