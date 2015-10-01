/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL
#define DILAY_TOOL

#include <glm/fwd.hpp>
#include "macro.hpp"
#include "sketch/fwd.hpp"

class CacheProxy;
class Config;
enum class Dimension;
class Intersection;
class Mirror;
class QMouseEvent;
class QWheelEvent;
class State;
class ViewProperties;
class ViewToolTip;
class WingedFaceIntersection;

enum class ToolResponse {
  None, Terminate, Redraw
};

class Tool {
  public:
    DECLARE_BIG3_VIRTUAL (Tool, State&, const char*)

    ToolResponse     initialize             ();
    void             render                 () const;
    ToolResponse     mouseMoveEvent         (const QMouseEvent&);
    ToolResponse     mousePressEvent        (const QMouseEvent&);
    ToolResponse     mouseReleaseEvent      (const QMouseEvent&);
    ToolResponse     wheelEvent             (const QWheelEvent&);
    void             close                  ();

  protected:
    State&           state                  () const;
    void             updateGlWidget         ();
    ViewProperties&  properties             () const;
    void             showToolTip            (const ViewToolTip&);
    Config&          config                 () const;
    CacheProxy&      cache                  ();
    CacheProxy       cache                  (const char*) const;
    glm::ivec2       cursorPosition         () const;
    void             snapshotAll            ();
    void             snapshotWingedMeshes   ();
    void             snapshotSketchMeshes   ();
    bool             intersectsRecentOctree (const QMouseEvent&, Intersection&) const;
    bool             hasMirror              () const;
    const Mirror&    mirror                 () const;
    void             mirror                 (bool);
    void             renderMirror           (bool);
    const Dimension* mirrorDimension        () const;
    void             mirrorWingedMeshes     ();
    void             mirrorSketchMeshes     ();

    template <typename T, typename ... Ts>
    bool intersectsScene (const glm::ivec2&, T&, Ts ...);
    template <typename T, typename ... Ts>
    bool intersectsScene (const QMouseEvent&, T&, Ts ...);

  private:
    IMPLEMENTATION

    virtual const char*  key                  () const = 0;
    virtual ToolResponse runInitialize        ()                   { return ToolResponse::None; }
    virtual void         runRender            () const             {}
    virtual ToolResponse runMouseMoveEvent    (const QMouseEvent&) { return ToolResponse::None; }
    virtual ToolResponse runMousePressEvent   (const QMouseEvent&) { return ToolResponse::None; }
    virtual ToolResponse runMouseReleaseEvent (const QMouseEvent&) { return ToolResponse::None; }
    virtual ToolResponse runWheelEvent        (const QWheelEvent&) { return ToolResponse::None; }
    virtual void         runClose             ()                   {}
};

#define DECLARE_TOOL(name,theKey,otherMethods)                       \
  class name : public Tool { public:                                 \
    DECLARE_BIG2 (name, State&)                                      \
    private:                                                         \
      IMPLEMENTATION                                                 \
      const char* key () const { return theKey ; }                   \
      otherMethods };

#define DECLARE_TOOL_RUN_INITIALIZE          ToolResponse runInitialize        ();
#define DECLARE_TOOL_RUN_RENDER              void         runRender            () const;
#define DECLARE_TOOL_RUN_MOUSE_MOVE_EVENT    ToolResponse runMouseMoveEvent    (const QMouseEvent&);
#define DECLARE_TOOL_RUN_MOUSE_PRESS_EVENT   ToolResponse runMousePressEvent   (const QMouseEvent&);
#define DECLARE_TOOL_RUN_MOUSE_RELEASE_EVENT ToolResponse runMouseReleaseEvent (const QMouseEvent&);
#define DECLARE_TOOL_RUN_MOUSE_WHEEL_EVENT   ToolResponse runWheelEvent        (const QWheelEvent&);
#define DECLARE_TOOL_RUN_CLOSE               void         runClose             ();

#define DELEGATE_TOOL(name) \
  DELEGATE_BIG2_BASE (name, (State& s), (this), Tool, (s, this->key ()))

#define DELEGATE_TOOL_RUN_INITIALIZE(n)          DELEGATE       (ToolResponse, n, runInitialize)
#define DELEGATE_TOOL_RUN_RENDER(n)              DELEGATE_CONST (void        , n, runRender)
#define DELEGATE_TOOL_RUN_MOUSE_MOVE_EVENT(n)    DELEGATE1      (ToolResponse, n, runMouseMoveEvent, const QMouseEvent&)
#define DELEGATE_TOOL_RUN_MOUSE_PRESS_EVENT(n)   DELEGATE1      (ToolResponse, n, runMousePressEvent, const QMouseEvent&)
#define DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT(n) DELEGATE1      (ToolResponse, n, runMouseReleaseEvent, const QMouseEvent&)
#define DELEGATE_TOOL_RUN_MOUSE_WHEEL_EVENT(n)   DELEGATE1      (ToolResponse, n, runWheelEvent, const QWheelEvent&)
#define DELEGATE_TOOL_RUN_CLOSE(n)               DELEGATE       (void        , n, runClose)

#endif
