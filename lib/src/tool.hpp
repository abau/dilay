#ifndef DILAY_TOOL
#define DILAY_TOOL

#include <glm/fwd.hpp>
#include "macro.hpp"

class ConfigProxy;
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
    ToolResponse                  mouseMoveEvent       (QMouseEvent&);
    ToolResponse                  mousePressEvent      (QMouseEvent&);
    ToolResponse                  mouseReleaseEvent    (QMouseEvent&);
    ToolResponse                  wheelEvent           (QWheelEvent&);
    void                          close                ();

  protected:
    State&                        state                () const;
    void                          updateGlWidget       ();
    ViewProperties&               properties           () const;
    ViewToolTip&                  toolTip              () const;
    void                          resetToolTip         ();
    ConfigProxy&                  config               () const;
    glm::ivec2                    cursorPosition       () const;

  private:
    IMPLEMENTATION

    virtual const char*  key                  () const = 0;
    virtual bool         runAllowUndoRedo     () const       { return false; }
    virtual ToolResponse runInitialize        ()             { return ToolResponse::None; }
    virtual void         runRender            () const       {}
    virtual ToolResponse runMouseMoveEvent    (QMouseEvent&) { return ToolResponse::None; }
    virtual ToolResponse runMousePressEvent   (QMouseEvent&) { return ToolResponse::None; }
    virtual ToolResponse runMouseReleaseEvent (QMouseEvent&) { return ToolResponse::None; }
    virtual ToolResponse runWheelEvent        (QWheelEvent&) { return ToolResponse::None; }
    virtual void         runClose             ()             {}
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
#define DECLARE_TOOL_RUN_MOUSE_MOVE_EVENT    ToolResponse runMouseMoveEvent    (QMouseEvent&);
#define DECLARE_TOOL_RUN_MOUSE_PRESS_EVENT   ToolResponse runMousePressEvent   (QMouseEvent&);
#define DECLARE_TOOL_RUN_MOUSE_RELEASE_EVENT ToolResponse runMouseReleaseEvent (QMouseEvent&);
#define DECLARE_TOOL_RUN_MOUSE_WHEEL_EVENT   ToolResponse runWheelEvent        (QWheelEvent&);
#define DECLARE_TOOL_RUN_CLOSE               void         runClose             ();

#define DELEGATE_TOOL(name)\
  DELEGATE_BIG2_BASE (name, (const ViewToolMenuParameters& p), (this), Tool, (p, this->key ()))

#define DELEGATE_TOOL_RUN_INITIALIZE(n)          DELEGATE       (ToolResponse, n, runInitialize)
#define DELEGATE_TOOL_RUN_RENDER(n)              DELEGATE_CONST (void        , n, runRender)
#define DELEGATE_TOOL_RUN_MOUSE_MOVE_EVENT(n)    DELEGATE1      (ToolResponse, n, runMouseMoveEvent, QMouseEvent&)
#define DELEGATE_TOOL_RUN_MOUSE_PRESS_EVENT(n)   DELEGATE1      (ToolResponse, n, runMousePressEvent, QMouseEvent&)
#define DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT(n) DELEGATE1      (ToolResponse, n, runMouseReleaseEvent, QMouseEvent&)
#define DELEGATE_TOOL_RUN_MOUSE_WHEEL_EVENT(n)   DELEGATE1      (ToolResponse, n, runWheelEvent, QWheelEvent&)
#define DELEGATE_TOOL_RUN_CLOSE(n)               DELEGATE       (void        , n, runClose)

#endif
