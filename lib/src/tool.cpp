/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "cache.hpp"
#include "camera.hpp"
#include "dimension.hpp"
#include "dynamic/mesh.hpp"
#include "history.hpp"
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
#include "view/info-pane.hpp"
#include "view/main-window.hpp"
#include "view/pointing-event.hpp"
#include "view/tool-pane.hpp"
#include "view/tool-tip.hpp"

struct Tool::Impl
{
  Tool*         self;
  State&        state;
  QPushButton&  button;
  CacheProxy    _cache;
  Maybe<Mirror> _mirror;
  bool          renderMirror;

  Impl (Tool* s, State& st, QPushButton& b, const char* key)
    : self (s)
    , state (st)
    , button (b)
    , _cache (this->cache (key))
    , renderMirror (true)
  {
    this->state.mainWindow ().infoPane ().showToolTip (ViewToolTip ());
    this->mirror (this->hasMirror ());
  }

  ToolResponse initialize ()
  {
    this->fromConfig ();
    return this->self->runInitialize ();
  }

  void render () const
  {
    this->self->runRender ();
    if (this->_mirror && this->renderMirror)
    {
      this->_mirror->render (this->state.camera ());
    }
  }

  void paint (QPainter& painter) const
  {
    this->self->runPaint (painter);
  }

  ToolResponse pointingEvent (const ViewPointingEvent& e)
  {
    ToolResponse response = this->self->runPointingEvent (e);

    if (e.releaseEvent ())
    {
      this->state.scene ().sanitizeMeshes ();
    }
    return response;
  }

  ToolResponse wheelEvent (const QWheelEvent& e)
  {
    return this->self->runWheelEvent (e);
  }

  ToolResponse cursorUpdate (const glm::ivec2& pos)
  {
    return this->self->runCursorUpdate (pos);
  }

  void close ()
  {
    return this->self->runClose ();
  }

  void fromConfig ()
  {
    if (this->hasMirror ())
    {
      this->_mirror->fromConfig (this->config ());
    }
    this->self->runFromConfig ();
  }

  void updateGlWidget ()
  {
    this->state.mainWindow ().glWidget ().update ();
  }

  ViewTwoColumnGrid& makeProperties () const
  {
    return this->state.mainWindow ().toolPane ().makeProperties ();
  }

  void showToolTip (const ViewToolTip& toolTip)
  {
    this->state.mainWindow ().infoPane ().showToolTip (toolTip);
  }

  Config& config () const
  {
    return this->state.config ();
  }

  CacheProxy& cache ()
  {
    return this->_cache;
  }

  CacheProxy cache (const char* key) const
  {
    return CacheProxy (this->state.cache (), "editor/tool/" + std::string (key) + "/");
  }

  glm::ivec2 cursorPosition ()
  {
    return this->state.mainWindow ().glWidget ().cursorPosition ();
  }

  void snapshotAll ()
  {
    this->state.history ().snapshotAll (this->state.scene ());
  }

  void snapshotDynamicMeshes ()
  {
    this->state.history ().snapshotDynamicMeshes (this->state.scene ());
  }

  void snapshotSketchMeshes ()
  {
    this->state.history ().snapshotSketchMeshes (this->state.scene ());
  }

  bool intersectsRecentDynamicMesh (const PrimRay& ray, Intersection& intersection) const
  {
    assert (this->state.history ().hasRecentDynamicMesh ());

    this->state.history ().forEachRecentDynamicMesh (
      [&ray, &intersection](const DynamicMesh& mesh) { mesh.intersects (ray, intersection); });
    return intersection.isIntersection ();
  }

  bool intersectsRecentDynamicMesh (const glm::ivec2& pos, Intersection& intersection) const
  {
    return this->intersectsRecentDynamicMesh (this->state.camera ().ray (pos), intersection);
  }

  bool hasMirror () const
  {
    return this->state.cache ().get ("editor/tool/mirror", true);
  }

  const Mirror& mirror () const
  {
    assert (this->hasMirror ());
    return *this->_mirror;
  }

  void mirror (bool m)
  {
    this->state.cache ().set ("editor/tool/mirror", m);

    if (m)
    {
      this->_mirror = Maybe<Mirror>::make (this->config (), Dimension::X);
    }
    else
    {
      this->_mirror.reset ();
    }
    this->updateGlWidget ();
  }

  const Dimension* mirrorDimension () const
  {
    const static Dimension d = Dimension::X;

    return this->hasMirror () ? &d : nullptr;
  }

  void mirrorDynamicMeshes ()
  {
    assert (this->hasMirror ());

    this->snapshotDynamicMeshes ();
    this->state.scene ().forEachMesh ([this](DynamicMesh& mesh) {
      mesh.mirror (this->mirror ().plane ());
      mesh.bufferData ();
    });
  }

  void mirrorSketchMeshes ()
  {
    assert (this->hasMirror ());

    this->snapshotSketchMeshes ();
    this->state.scene ().forEachMesh (
      [this](SketchMesh& mesh) { mesh.mirror (*this->mirrorDimension ()); });
  }

  template <typename T, typename... Ts>
  bool intersectsScene (const PrimRay& ray, T& intersection, Ts... args)
  {
    return this->state.scene ().intersects (ray, intersection, std::forward<Ts> (args)...);
  }

  template <typename T, typename... Ts>
  bool intersectsScene (const glm::ivec2& pos, T& intersection, Ts... args)
  {
    return this->intersectsScene (this->state.camera ().ray (pos), intersection,
                                  std::forward<Ts> (args)...);
  }

  template <typename T, typename... Ts>
  bool intersectsScene (const ViewPointingEvent& e, T& intersection, Ts... args)
  {
    return this->intersectsScene (e.ivec2 (), intersection, std::forward<Ts> (args)...);
  }

  ToolResponse runPointingEvent (const ViewPointingEvent& e)
  {
    if (e.pressEvent ())
    {
      return this->self->runPressEvent (e);
    }
    else if (e.moveEvent ())
    {
      return this->self->runMoveEvent (e);
    }
    else if (e.releaseEvent ())
    {
      return this->self->runReleaseEvent (e);
    }
    return ToolResponse::None;
  }
};

DELEGATE3_BIG3_SELF (Tool, State&, QPushButton&, const char*)
GETTER_CONST (QPushButton&, Tool, button);
DELEGATE (ToolResponse, Tool, initialize)
DELEGATE_CONST (void, Tool, render)
DELEGATE1_CONST (void, Tool, paint, QPainter&)
DELEGATE1 (ToolResponse, Tool, pointingEvent, const ViewPointingEvent&)
DELEGATE1 (ToolResponse, Tool, wheelEvent, const QWheelEvent&)
DELEGATE1 (ToolResponse, Tool, cursorUpdate, const glm::ivec2&)
DELEGATE (void, Tool, close)
DELEGATE (void, Tool, fromConfig)
GETTER_CONST (State&, Tool, state)
DELEGATE (void, Tool, updateGlWidget)
DELEGATE_CONST (ViewTwoColumnGrid&, Tool, makeProperties)
DELEGATE1 (void, Tool, showToolTip, const ViewToolTip&)
DELEGATE_CONST (Config&, Tool, config)
DELEGATE (CacheProxy&, Tool, cache)
DELEGATE1_CONST (CacheProxy, Tool, cache, const char*)
DELEGATE_CONST (glm::ivec2, Tool, cursorPosition)
DELEGATE (void, Tool, snapshotAll)
DELEGATE (void, Tool, snapshotDynamicMeshes)
DELEGATE (void, Tool, snapshotSketchMeshes)
DELEGATE2_CONST (bool, Tool, intersectsRecentDynamicMesh, const PrimRay&, Intersection&)
DELEGATE2_CONST (bool, Tool, intersectsRecentDynamicMesh, const glm::ivec2&, Intersection&)
DELEGATE_CONST (bool, Tool, hasMirror)
DELEGATE_CONST (const Mirror&, Tool, mirror)
DELEGATE1 (void, Tool, mirror, bool)
SETTER (bool, Tool, renderMirror)
DELEGATE_CONST (const Dimension*, Tool, mirrorDimension)
DELEGATE (void, Tool, mirrorDynamicMeshes)
DELEGATE (void, Tool, mirrorSketchMeshes)
DELEGATE1 (ToolResponse, Tool, runPointingEvent, const ViewPointingEvent&)

template <typename T, typename... Ts>
bool Tool::intersectsScene (const PrimRay& ray, T& intersection, Ts... args)
{
  return this->impl->intersectsScene (ray, intersection, std::forward<Ts> (args)...);
}

template <typename T, typename... Ts>
bool Tool::intersectsScene (const glm::ivec2& pos, T& intersection, Ts... args)
{
  return this->impl->intersectsScene (pos, intersection, std::forward<Ts> (args)...);
}

template <typename T, typename... Ts>
bool Tool::intersectsScene (const ViewPointingEvent& e, T& intersection, Ts... args)
{
  return this->impl->intersectsScene (e, intersection, std::forward<Ts> (args)...);
}

template bool Tool::intersectsScene (const PrimRay&, DynamicMeshIntersection&);
template bool Tool::intersectsScene (const glm::ivec2&, DynamicMeshIntersection&);
template bool Tool::intersectsScene (const ViewPointingEvent&, DynamicMeshIntersection&);
template bool Tool::intersectsScene (const PrimRay&, SketchNodeIntersection&);
template bool Tool::intersectsScene (const glm::ivec2&, SketchNodeIntersection&);
template bool Tool::intersectsScene (const ViewPointingEvent&, SketchNodeIntersection&);
template bool Tool::intersectsScene (const PrimRay&, SketchBoneIntersection&);
template bool Tool::intersectsScene (const glm::ivec2&, SketchBoneIntersection&);
template bool Tool::intersectsScene (const ViewPointingEvent&, SketchBoneIntersection&);
template bool Tool::intersectsScene (const PrimRay&, SketchMeshIntersection&);
template bool Tool::intersectsScene (const glm::ivec2&, SketchMeshIntersection&);
template bool Tool::intersectsScene (const ViewPointingEvent&, SketchMeshIntersection&);
template bool Tool::intersectsScene (const PrimRay&, SketchMeshIntersection&, unsigned int);
template bool Tool::intersectsScene (const glm::ivec2&, SketchMeshIntersection&, unsigned int);
template bool Tool::intersectsScene (const ViewPointingEvent&, SketchMeshIntersection&,
                                     unsigned int);
template bool Tool::intersectsScene (const PrimRay&, SketchPathIntersection&);
template bool Tool::intersectsScene (const glm::ivec2&, SketchPathIntersection&);
template bool Tool::intersectsScene (const ViewPointingEvent&, SketchPathIntersection&);
template bool Tool::intersectsScene (const PrimRay&, Intersection&);
template bool Tool::intersectsScene (const glm::ivec2&, Intersection&);
template bool Tool::intersectsScene (const ViewPointingEvent&, Intersection&);
