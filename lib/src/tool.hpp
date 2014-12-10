#ifndef DILAY_TOOL
#define DILAY_TOOL

#include <QString>
#include "macro.hpp"

class ViewProperties;
class ViewToolMenuParameters;
class QMouseEvent;
class QWheelEvent;

enum class ToolResponse {
  None, Terminate, Redraw
};

class Tool {
  public:
    DECLARE_BIG3_VIRTUAL (Tool, const ViewToolMenuParameters&)

    const ViewToolMenuParameters& menuParameters       () const;
    ToolResponse                  initialize           ();
    void                          render               ();
    ToolResponse                  mouseMoveEvent       (QMouseEvent&);
    ToolResponse                  mousePressEvent      (QMouseEvent&);
    ToolResponse                  mouseReleaseEvent    (QMouseEvent&);
    ToolResponse                  wheelEvent           (QWheelEvent&);
    QString                       message              () const;
    void                          close                ();
    void                          cancel               ();

  protected:
    void                          updateGlWidget       ();
    ViewProperties&               properties           ();

  private:
    IMPLEMENTATION

    virtual ToolResponse runInitialize        ()             { return ToolResponse::None; }
    virtual void         runRender            ()             {}
    virtual ToolResponse runMouseMoveEvent    (QMouseEvent&) { return ToolResponse::None; }
    virtual ToolResponse runMousePressEvent   (QMouseEvent&) { return ToolResponse::None; }
    virtual ToolResponse runMouseReleaseEvent (QMouseEvent&) { return ToolResponse::None; }
    virtual ToolResponse runWheelEvent        (QWheelEvent&) { return ToolResponse::None; }
    virtual QString      runMessage           () const       { return QString (); }
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
#define DECLARE_TOOL_RUN_MESSAGE             QString      runMessage           () const;
#define DECLARE_TOOL_RUN_CLOSE               void         runClose             ();

#define DELEGATE_TOOL(name)\
  DELEGATE_BIG3_BASE ( name, (const ViewToolMenuParameters& p), (this), Tool, (p))

#define DELEGATE_TOOL_RUN_INITIALIZE(n)          DELEGATE       (ToolResponse, n, runInitialize)
#define DELEGATE_TOOL_RUN_RENDER(n)              DELEGATE       (void        , n, runRender)
#define DELEGATE_TOOL_RUN_MOUSE_MOVE_EVENT(n)    DELEGATE1      (ToolResponse, n, runMouseMoveEvent, QMouseEvent&)
#define DELEGATE_TOOL_RUN_MOUSE_PRESS_EVENT(n)   DELEGATE1      (ToolResponse, n, runMousePressEvent, QMouseEvent&)
#define DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT(n) DELEGATE1      (ToolResponse, n, runMouseReleaseEvent, QMouseEvent&)
#define DELEGATE_TOOL_RUN_MOUSE_WHEEL_EVENT(n)   DELEGATE1      (ToolResponse, n, runWheelEvent, QWheelEvent&)
#define DELEGATE_TOOL_RUN_MESSAGE(n)             DELEGATE_CONST (QString     , n, runMessage)
#define DELEGATE_TOOL_RUN_CLOSE(n)               DELEGATE       (void        , n, runClose)

#endif
