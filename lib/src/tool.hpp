/* This file is part of Dilay
 * Copyright © 2015-2018 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL
#define DILAY_TOOL

#include <glm/fwd.hpp>
#include "macro.hpp"
#include "sketch/fwd.hpp"
#include "tool/key.hpp"

class CacheProxy;
class Config;
enum class Dimension;
class Intersection;
class Mirror;
class PrimRay;
class QPainter;
class QWidget;
class State;
class ToolUtilMovement;
class ViewKeyEvent;
class ViewPointingEvent;
class ViewToolTip;
class ViewTwoColumnGrid;

enum class ToolResponse
{
  None,
  Terminate,
  Redraw
};

class Tool
{
public:
  DECLARE_BIG3_VIRTUAL (Tool, State&, const char*)

  virtual ToolKey getKey () const = 0;

  ToolResponse initialize ();
  void         render () const;
  void         paint (QPainter&) const;
  void         keyEvent (const ViewKeyEvent&);
  ToolResponse pointingEvent (const ViewPointingEvent&);
  ToolResponse cursorUpdate (const glm::ivec2&);
  ToolResponse commit ();
  void         fromConfig ();

protected:
  State&             state () const;
  void               updateGlWidget ();
  ViewTwoColumnGrid& properties () const;
  Config&            config () const;
  CacheProxy&        cache ();
  CacheProxy         cache (const char*) const;
  glm::ivec2         cursorPosition () const;
  void               snapshotAll ();
  void               snapshotDynamicMeshes ();
  void               snapshotSketchMeshes ();
  bool               intersectsRecentDynamicMesh (const PrimRay&, Intersection&) const;
  bool               intersectsRecentDynamicMesh (const glm::ivec2&, Intersection&) const;
  void               supportsMirror ();
  bool               mirrorEnabled () const;
  void               mirrorPosition (const glm::vec3&);
  void               mirrorDefaultPosition ();
  const Mirror&      mirror () const;
  const Dimension*   mirrorDimension () const;
  void               addMirrorProperties ();
  void               enableMirrorProperties (bool = true);
  void               addMoveOnPrimaryPlaneProperties (ToolUtilMovement&);
  bool               onKeymap (char) const;

  template <typename T, typename... Ts> bool intersectsScene (const PrimRay&, T&, Ts...);
  template <typename T, typename... Ts> bool intersectsScene (const glm::ivec2&, T&, Ts...);
  template <typename T, typename... Ts> bool intersectsScene (const ViewPointingEvent&, T&, Ts...);

private:
  IMPLEMENTATION

  virtual ToolResponse runInitialize () = 0;

  virtual void runRender () const {}

  virtual void runPaint (QPainter&) const {}

  virtual ToolResponse runPointingEvent (const ViewPointingEvent&);

  virtual ToolResponse runPressEvent (const ViewPointingEvent&) { return ToolResponse::None; }

  virtual ToolResponse runMoveEvent (const ViewPointingEvent&) { return ToolResponse::None; }

  virtual ToolResponse runReleaseEvent (const ViewPointingEvent&) { return ToolResponse::None; }

  virtual ToolResponse runCursorUpdate (const glm::ivec2&) { return ToolResponse::None; }

  virtual ToolResponse runCommit () { return ToolResponse::None; }

  virtual void runFromConfig () {}
};

#define DECLARE_TOOL(keyName, otherMethods)              \
  class Tool##keyName : public Tool                      \
  {                                                      \
  public:                                                \
    DECLARE_BIG2 (Tool##keyName, State&)                 \
                                                         \
    ToolKey getKey () const { return ToolKey::keyName; } \
                                                         \
  private:                                               \
    IMPLEMENTATION                                       \
    ToolResponse runInitialize ();                       \
    otherMethods                                         \
  };

#define DECLARE_TOOL_RUN_RENDER void runRender () const;
#define DECLARE_TOOL_RUN_PAINT void runPaint (QPainter&) const;
#define DECLARE_TOOL_RUN_POINTING_EVENT ToolResponse runPointingEvent (const ViewPointingEvent&);
#define DECLARE_TOOL_RUN_PRESS_EVENT ToolResponse runPressEvent (const ViewPointingEvent&);
#define DECLARE_TOOL_RUN_MOVE_EVENT ToolResponse runMoveEvent (const ViewPointingEvent&);
#define DECLARE_TOOL_RUN_RELEASE_EVENT ToolResponse runReleaseEvent (const ViewPointingEvent&);
#define DECLARE_TOOL_RUN_CURSOR_UPDATE ToolResponse runCursorUpdate (const glm::ivec2&);
#define DECLARE_TOOL_RUN_COMMIT ToolResponse runCommit ();
#define DECLARE_TOOL_RUN_FROM_CONFIG void runFromConfig ();

#define DELEGATE_TOOL(name)                                        \
  DELEGATE_BIG2_BASE (name, (State & s), (this), Tool, (s, #name)) \
  DELEGATE (ToolResponse, name, runInitialize)

#define DELEGATE_TOOL_RUN_RENDER(n) DELEGATE_CONST (void, n, runRender)
#define DELEGATE_TOOL_RUN_PAINT(n) DELEGATE1_CONST (void, n, runPaint, QPainter&)
#define DELEGATE_TOOL_RUN_POINTING_EVENT(n) \
  DELEGATE1 (ToolResponse, n, runPointingEvent, const ViewPointingEvent&)
#define DELEGATE_TOOL_RUN_PRESS_EVENT(n) \
  DELEGATE1 (ToolResponse, n, runPressEvent, const ViewPointingEvent&)
#define DELEGATE_TOOL_RUN_MOVE_EVENT(n) \
  DELEGATE1 (ToolResponse, n, runMoveEvent, const ViewPointingEvent&)
#define DELEGATE_TOOL_RUN_RELEASE_EVENT(n) \
  DELEGATE1 (ToolResponse, n, runReleaseEvent, const ViewPointingEvent&)
#define DELEGATE_TOOL_RUN_CURSOR_UPDATE(n) \
  DELEGATE1 (ToolResponse, n, runCursorUpdate, const glm::ivec2&)
#define DELEGATE_TOOL_RUN_COMMIT(n) DELEGATE (ToolResponse, n, runCommit)
#define DELEGATE_TOOL_RUN_FROM_CONFIG(n) DELEGATE (void, n, runFromConfig)

#endif
