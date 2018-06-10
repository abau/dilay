/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCheckBox>
#include <QPushButton>
#include <array>
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
#include "tool/util/movement.hpp"
#include "view/gl-widget.hpp"
#include "view/key-event.hpp"
#include "view/main-window.hpp"
#include "view/pointing-event.hpp"
#include "view/tool-pane.hpp"
#include "view/tool-tip.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

struct Tool::Impl
{
  Tool*                self;
  State&               state;
  CacheProxy           _cache;
  Maybe<Mirror>        _mirror;
  QCheckBox*           mirrorCheckBox;
  QPushButton*         mirrorSyncButton;
  glm::ivec2           prevPointingEventPosition;
  std::array<bool, 26> keymap;

  Impl (Tool* s, State& st, const char* cacheKey)
    : self (s)
    , state (st)
    , _cache (this->cache (cacheKey))
    , mirrorCheckBox (nullptr)
    , mirrorSyncButton (nullptr)
  {
    this->keymap.fill (false);
  }

  ToolResponse initialize ()
  {
    this->fromConfig ();
#ifndef NDEBUG
    const ToolResponse response = this->self->runInitialize ();
    assert (bool(this->_mirror) == false || (this->mirrorCheckBox && this->mirrorSyncButton));
    return response;
#else
    return this->self->runInitialize ();
#endif
  }

  void render () const
  {
    this->self->runRender ();
    if (this->mirrorEnabled ())
    {
      this->_mirror->render (this->state.camera ());
    }
  }

  void paint (QPainter& painter) const { this->self->runPaint (painter); }

  void keyEvent (const ViewKeyEvent& e)
  {
    const int index = int(e.key ()) - int(Qt::Key_A);
    if (index >= 0 && size_t (index) < this->keymap.size ())
    {
      this->keymap[index] = e.pressEvent ();
    }
  }

  ToolResponse pointingEvent (const ViewPointingEvent& e)
  {
    if (e.pressEvent ())
    {
      this->prevPointingEventPosition = e.position ();
    }

    const ViewPointingEvent eWithDelta (e, this->prevPointingEventPosition);
    ToolResponse            response = this->self->runPointingEvent (eWithDelta);

    if (e.releaseEvent ())
    {
      this->state.scene ().sanitizeMeshes ();
    }
    this->prevPointingEventPosition = e.position ();
    return response;
  }

  ToolResponse wheelEvent (const QWheelEvent& e) { return this->self->runWheelEvent (e); }

  ToolResponse cursorUpdate (const glm::ivec2& pos) { return this->self->runCursorUpdate (pos); }

  ToolResponse commit () { return this->self->runCommit (); }

  void fromConfig ()
  {
    if (this->_mirror)
    {
      this->_mirror->fromConfig (this->config ());
    }
    this->self->runFromConfig ();
  }

  void updateGlWidget () { this->state.mainWindow ().glWidget ().update (); }

  ViewTwoColumnGrid& properties () const
  {
    return this->state.mainWindow ().toolPane ().properties ();
  }

  Config& config () const { return this->state.config (); }

  CacheProxy& cache () { return this->_cache; }

  CacheProxy cache (const char* key) const
  {
    return CacheProxy (this->state.cache (), "editor/tool/" + std::string (key) + "/");
  }

  glm::ivec2 cursorPosition () { return this->state.mainWindow ().glWidget ().cursorPosition (); }

  void snapshotAll () { this->state.history ().snapshotAll (this->state.scene ()); }

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

  void supportsMirror ()
  {
    assert (bool(this->_mirror) == false);
    this->_mirror = Maybe<Mirror>::make (this->config (), Dimension::X);
  }

  bool mirrorEnabled () const
  {
    return this->_mirror && this->state.cache ().get ("editor/tool/mirror", true);
  }

  const Mirror& mirror () const
  {
    assert (this->mirrorEnabled ());
    return *this->_mirror;
  }

  const Dimension* mirrorDimension () const
  {
    const static Dimension d = Dimension::X;

    return this->mirrorEnabled () ? &d : nullptr;
  }

  void mirrorDynamicMeshes ()
  {
    assert (this->mirrorEnabled ());

    this->snapshotDynamicMeshes ();
    this->state.scene ().forEachMesh ([this](DynamicMesh& mesh) {
      if (mesh.mirror (this->mirror ().plane ()) == false)
      {
        ViewUtil::error (this->state.mainWindow (), QObject::tr ("Could not mirror mesh."));
      }
      mesh.bufferData ();
    });
  }

  void mirrorSketchMeshes ()
  {
    assert (this->mirrorEnabled ());

    this->snapshotSketchMeshes ();
    this->state.scene ().forEachMesh (
      [this](SketchMesh& mesh) { mesh.mirror (*this->mirrorDimension ()); });
  }

  void addMirrorProperties ()
  {
    assert (this->_mirror);

    this->mirrorCheckBox = &ViewUtil::checkBox (QObject::tr ("Mirror"), this->mirrorEnabled ());
    ViewUtil::connect (*this->mirrorCheckBox, [this](bool m) {
      this->state.cache ().set ("editor/tool/mirror", m);
      this->mirrorSyncButton->setEnabled (m);
      this->updateGlWidget ();
    });

    this->mirrorSyncButton = &ViewUtil::pushButton (QObject::tr ("Sync"));
    ViewUtil::connect (*this->mirrorSyncButton, [this]() {
      switch (this->state.mainWindow ().toolPane ().selection ())
      {
        case ViewToolPaneSelection::Sculpt:
          this->mirrorDynamicMeshes ();
          break;
        case ViewToolPaneSelection::Sketch:
          this->mirrorSketchMeshes ();
          break;
      }
      this->updateGlWidget ();
    });
    this->mirrorSyncButton->setEnabled (this->mirrorEnabled ());

    this->properties ().add (*this->mirrorCheckBox, *this->mirrorSyncButton);
  }

  void enableMirrorProperties (bool state)
  {
    assert (this->mirrorCheckBox);
    assert (this->mirrorSyncButton);

    this->mirrorCheckBox->setEnabled (state);
    this->mirrorSyncButton->setEnabled (state);
  }

  void addMoveOnPrimaryPlaneProperties (ToolUtilMovement& movement)
  {
    const bool init =
      this->cache ().get<bool> ("move-on-primary-plane", movement.onPrimaryPlane ());
    movement.onPrimaryPlane (init);

    QCheckBox& edit = ViewUtil::checkBox (QObject::tr ("Move on primary plane"), init);
    ViewUtil::connect (edit, [this, &movement](bool p) {
      movement.onPrimaryPlane (p);
      this->cache ().set ("move-on-primary-plane", p);
    });
    this->properties ().add (edit);
  }

  bool onKeymap (char c) const
  {
    const int index = int(c) - int('a');
    if (index >= 0 && size_t (index) < this->keymap.size ())
    {
      return this->keymap[index];
    }
    else
    {
      return false;
    }
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
    return this->intersectsScene (e.position (), intersection, std::forward<Ts> (args)...);
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

DELEGATE2_BIG3_SELF (Tool, State&, const char*)
DELEGATE (ToolResponse, Tool, initialize)
DELEGATE_CONST (void, Tool, render)
DELEGATE1_CONST (void, Tool, paint, QPainter&)
DELEGATE1 (void, Tool, keyEvent, const ViewKeyEvent&)
DELEGATE1 (ToolResponse, Tool, pointingEvent, const ViewPointingEvent&)
DELEGATE1 (ToolResponse, Tool, wheelEvent, const QWheelEvent&)
DELEGATE1 (ToolResponse, Tool, cursorUpdate, const glm::ivec2&)
DELEGATE (ToolResponse, Tool, commit)
DELEGATE (void, Tool, fromConfig)
GETTER_CONST (State&, Tool, state)
DELEGATE (void, Tool, updateGlWidget)
DELEGATE_CONST (ViewTwoColumnGrid&, Tool, properties)
DELEGATE_CONST (Config&, Tool, config)
DELEGATE (CacheProxy&, Tool, cache)
DELEGATE1_CONST (CacheProxy, Tool, cache, const char*)
DELEGATE_CONST (glm::ivec2, Tool, cursorPosition)
DELEGATE (void, Tool, snapshotAll)
DELEGATE (void, Tool, snapshotDynamicMeshes)
DELEGATE (void, Tool, snapshotSketchMeshes)
DELEGATE2_CONST (bool, Tool, intersectsRecentDynamicMesh, const PrimRay&, Intersection&)
DELEGATE2_CONST (bool, Tool, intersectsRecentDynamicMesh, const glm::ivec2&, Intersection&)
DELEGATE (void, Tool, supportsMirror)
DELEGATE_CONST (bool, Tool, mirrorEnabled)
DELEGATE_CONST (const Mirror&, Tool, mirror)
DELEGATE_CONST (const Dimension*, Tool, mirrorDimension)
DELEGATE (void, Tool, addMirrorProperties)
DELEGATE1 (void, Tool, enableMirrorProperties, bool)
DELEGATE1 (void, Tool, addMoveOnPrimaryPlaneProperties, ToolUtilMovement&)
DELEGATE1_CONST (bool, Tool, onKeymap, char)
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

#define TOOL_INTERSECTS_SCENE(...)                                      \
  template bool Tool::intersectsScene (const PrimRay&, __VA_ARGS__);    \
  template bool Tool::intersectsScene (const glm::ivec2&, __VA_ARGS__); \
  template bool Tool::intersectsScene (const ViewPointingEvent&, __VA_ARGS__);

TOOL_INTERSECTS_SCENE (DynamicMeshIntersection&)
TOOL_INTERSECTS_SCENE (SketchNodeIntersection&)
TOOL_INTERSECTS_SCENE (SketchBoneIntersection&)
TOOL_INTERSECTS_SCENE (SketchMeshIntersection&)
TOOL_INTERSECTS_SCENE (SketchMeshIntersection&, unsigned int)
TOOL_INTERSECTS_SCENE (SketchPathIntersection&)
TOOL_INTERSECTS_SCENE (Intersection&)

#undef TOOL_INTERSECTS_SCENE
