#ifndef DILAY_TOOL
#define DILAY_TOOL

#include <glm/fwd.hpp>
#include <QString>
#include "macro.hpp"

class QMouseEvent;
class ViewMainWindow;
class ViewToolOptions;
class QWheelEvent;

enum class ToolResponse {
  None, Terminate, Redraw
};

class Tool {
  public:
    DECLARE_BIG3_VIRTUAL (Tool, ViewMainWindow&, const glm::ivec2&, const QString&)

    Tool (ViewMainWindow&, const glm::ivec2&);

    friend class ToolUtil;

    ViewMainWindow&    mainWindow         ();
    const glm::ivec2&  clickPosition      () const;

    ToolResponse       execute            ();
    void               render             ();
    ToolResponse       mouseMoveEvent     (QMouseEvent&);
    ToolResponse       mousePressEvent    (QMouseEvent&);
    ToolResponse       mouseReleaseEvent  (QMouseEvent&);
    ToolResponse       wheelEvent         (QWheelEvent&);
    QString            message            () const;

  protected:
    ViewToolOptions*   toolOptions        ();

  private:
    class Impl;
    Impl* impl;

    virtual ToolResponse runExecute           ()             { return ToolResponse::None; }
    virtual void         runRender            ()             {}
    virtual ToolResponse runMouseMoveEvent    (QMouseEvent&) { return ToolResponse::None; }
    virtual ToolResponse runMousePressEvent   (QMouseEvent&) { return ToolResponse::None; }
    virtual ToolResponse runMouseReleaseEvent (QMouseEvent&) { return ToolResponse::None; }
    virtual ToolResponse runWheelEvent        (QWheelEvent&) { return ToolResponse::None; }
    virtual QString      runMessage           () const       { return QString (); }
};

#endif
