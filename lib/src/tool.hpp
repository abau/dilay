#ifndef DILAY_TOOL
#define DILAY_TOOL

#include <QString>
#include "macro.hpp"

class ViewToolMenuParameters;
class ViewToolParameters;
class QMouseEvent;
class QWheelEvent;

enum class ToolResponse {
  None, Terminate, Redraw
};

class Tool {
  public:
    DECLARE_BIG3_VIRTUAL (Tool, const ViewToolMenuParameters&, const QString&)

    Tool (const ViewToolMenuParameters&);

    friend class ToolUtil;

    const ViewToolMenuParameters& menuParameters     () const;
    ToolResponse                  execute            ();
    void                          render             ();
    ToolResponse                  mouseMoveEvent     (QMouseEvent&);
    ToolResponse                  mousePressEvent    (QMouseEvent&);
    ToolResponse                  mouseReleaseEvent  (QMouseEvent&);
    ToolResponse                  wheelEvent         (QWheelEvent&);
    QString                       message            () const;

  protected:
    ViewToolParameters*           toolParameters     ();

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
