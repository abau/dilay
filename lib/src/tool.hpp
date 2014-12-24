#ifndef DILAY_TOOL
#define DILAY_TOOL

#include <string>
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
    DECLARE_BIG3_VIRTUAL (Tool, const ViewToolMenuParameters&, const std::string&)

    const ViewToolMenuParameters& menuParameters       () const;
    void                          showToolTip          ();
    ToolResponse                  initialize           ();
    void                          render               ();
    ToolResponse                  mouseMoveEvent       (QMouseEvent&);
    ToolResponse                  mousePressEvent      (QMouseEvent&);
    ToolResponse                  mouseReleaseEvent    (QMouseEvent&);
    ToolResponse                  wheelEvent           (QWheelEvent&);
    void                          close                ();

  protected:
    State&                        state                ();
    void                          updateGlWidget       ();
    ViewProperties&               properties           ();
    ViewToolTip&                  toolTip              ();
    void                          resetToolTip         ();
    ConfigProxy&                  config               ();

  private:
    IMPLEMENTATION

    virtual ToolResponse runInitialize        ()             { return ToolResponse::None; }
    virtual void         runRender            ()             {}
    virtual ToolResponse runMouseMoveEvent    (QMouseEvent&) { return ToolResponse::None; }
    virtual ToolResponse runMousePressEvent   (QMouseEvent&) { return ToolResponse::None; }
    virtual ToolResponse runMouseReleaseEvent (QMouseEvent&) { return ToolResponse::None; }
    virtual ToolResponse runWheelEvent        (QWheelEvent&) { return ToolResponse::None; }
    virtual void         runClose             ()             {}
};

#define DECLARE_TOOL(name,methods)                     \
  class name : public Tool { public:                   \
    DECLARE_BIG3 (name, const ViewToolMenuParameters&) \
    private: IMPLEMENTATION methods };

#define DECLARE_TOOL_RUN_INITIALIZE          ToolResponse runInitialize        ();
#define DECLARE_TOOL_RUN_RENDER              void         runRender            ();
#define DECLARE_TOOL_RUN_MOUSE_MOVE_EVENT    ToolResponse runMouseMoveEvent    (QMouseEvent&);
#define DECLARE_TOOL_RUN_MOUSE_PRESS_EVENT   ToolResponse runMousePressEvent   (QMouseEvent&);
#define DECLARE_TOOL_RUN_MOUSE_RELEASE_EVENT ToolResponse runMouseReleaseEvent (QMouseEvent&);
#define DECLARE_TOOL_RUN_MOUSE_WHEEL_EVENT   ToolResponse runWheelEvent        (QWheelEvent&);
#define DECLARE_TOOL_RUN_CLOSE               void         runClose             ();

#define DELEGATE_TOOL(name)\
  DELEGATE_BIG3_BASE (name, (const ViewToolMenuParameters& p), (this), Tool, (p, #name))

#define DELEGATE_TOOL_RUN_INITIALIZE(n)          DELEGATE       (ToolResponse, n, runInitialize)
#define DELEGATE_TOOL_RUN_RENDER(n)              DELEGATE       (void        , n, runRender)
#define DELEGATE_TOOL_RUN_MOUSE_MOVE_EVENT(n)    DELEGATE1      (ToolResponse, n, runMouseMoveEvent, QMouseEvent&)
#define DELEGATE_TOOL_RUN_MOUSE_PRESS_EVENT(n)   DELEGATE1      (ToolResponse, n, runMousePressEvent, QMouseEvent&)
#define DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT(n) DELEGATE1      (ToolResponse, n, runMouseReleaseEvent, QMouseEvent&)
#define DELEGATE_TOOL_RUN_MOUSE_WHEEL_EVENT(n)   DELEGATE1      (ToolResponse, n, runWheelEvent, QWheelEvent&)
#define DELEGATE_TOOL_RUN_CLOSE(n)               DELEGATE       (void        , n, runClose)

#endif
