#ifndef DILAY_TOOL
#define DILAY_TOOL

#include <glm/fwd.hpp>
#include "macro.hpp"

class CacheProxy;
class Config;
class State;
class ViewProperties;
class ViewToolMenuParameters;
class ViewToolTip;
class QMouseEvent;
class QWheelEvent;

enum class ToolResponse {
  None, Terminate, Redraw
};

class Tool {
  public:
    DECLARE_BIG3_VIRTUAL (Tool, const ViewToolMenuParameters&, const char*)

    const ViewToolMenuParameters& menuParameters       () const;
    void                          showToolTip          ();
    bool                          allowUndo            () const;
    bool                          allowRedo            () const;
    ToolResponse                  initialize           ();
    void                          render               () const;
    ToolResponse                  mouseMoveEvent       (const QMouseEvent&);
    ToolResponse                  mousePressEvent      (const QMouseEvent&);
    ToolResponse                  mouseReleaseEvent    (const QMouseEvent&);
    ToolResponse                  wheelEvent           (const QWheelEvent&);
    void                          close                ();

  protected:
    State&                        state                () const;
    void                          updateGlWidget       ();
    ViewProperties&               properties           () const;
    ViewToolTip&                  toolTip              () const;
    void                          resetToolTip         ();
    Config&                       config               () const;
    CacheProxy&                   cache                () const;
    glm::ivec2                    cursorPosition       () const;

  private:
    IMPLEMENTATION

    virtual const char*  key                  () const = 0;
    virtual bool         runAllowUndoRedo     () const             { return false; }
    virtual ToolResponse runInitialize        ()                   { return ToolResponse::None; }
    virtual void         runRender            () const             {}
    virtual ToolResponse runMouseMoveEvent    (const QMouseEvent&) { return ToolResponse::None; }
    virtual ToolResponse runMousePressEvent   (const QMouseEvent&) { return ToolResponse::None; }
    virtual ToolResponse runMouseReleaseEvent (const QMouseEvent&) { return ToolResponse::None; }
    virtual ToolResponse runWheelEvent        (const QWheelEvent&) { return ToolResponse::None; }
    virtual void         runClose             ()                   {}
};

#define DECLARE_TOOL(name,theKey,otherMethods)         \
  class name : public Tool { public:                   \
    DECLARE_BIG2 (name, const ViewToolMenuParameters&) \
    private:                                           \
      IMPLEMENTATION                                   \
      const char* key () const { return theKey ; }     \
      otherMethods };

#define ALLOW_TOOL_UNDO_REDO                 bool         runAllowUndoRedo     () const { return true; };
#define DECLARE_TOOL_RUN_INITIALIZE          ToolResponse runInitialize        ();
#define DECLARE_TOOL_RUN_RENDER              void         runRender            () const;
#define DECLARE_TOOL_RUN_MOUSE_MOVE_EVENT    ToolResponse runMouseMoveEvent    (const QMouseEvent&);
#define DECLARE_TOOL_RUN_MOUSE_PRESS_EVENT   ToolResponse runMousePressEvent   (const QMouseEvent&);
#define DECLARE_TOOL_RUN_MOUSE_RELEASE_EVENT ToolResponse runMouseReleaseEvent (const QMouseEvent&);
#define DECLARE_TOOL_RUN_MOUSE_WHEEL_EVENT   ToolResponse runWheelEvent        (const QWheelEvent&);
#define DECLARE_TOOL_RUN_CLOSE               void         runClose             ();

#define DELEGATE_TOOL(name)\
  DELEGATE_BIG2_BASE (name, (const ViewToolMenuParameters& p), (this), Tool, (p, this->key ()))

#define DELEGATE_TOOL_RUN_INITIALIZE(n)          DELEGATE       (ToolResponse, n, runInitialize)
#define DELEGATE_TOOL_RUN_RENDER(n)              DELEGATE_CONST (void        , n, runRender)
#define DELEGATE_TOOL_RUN_MOUSE_MOVE_EVENT(n)    DELEGATE1      (ToolResponse, n, runMouseMoveEvent, const QMouseEvent&)
#define DELEGATE_TOOL_RUN_MOUSE_PRESS_EVENT(n)   DELEGATE1      (ToolResponse, n, runMousePressEvent, const QMouseEvent&)
#define DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT(n) DELEGATE1      (ToolResponse, n, runMouseReleaseEvent, const QMouseEvent&)
#define DELEGATE_TOOL_RUN_MOUSE_WHEEL_EVENT(n)   DELEGATE1      (ToolResponse, n, runWheelEvent, const QWheelEvent&)
#define DELEGATE_TOOL_RUN_CLOSE(n)               DELEGATE       (void        , n, runClose)

#endif
