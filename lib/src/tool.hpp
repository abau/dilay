#ifndef DILAY_TOOL
#define DILAY_TOOL

#include <glm/fwd.hpp>
#include <QString>
#include "macro.hpp"

class QMouseEvent;
class ViewMainWindow;
class ViewToolOptions;
class QWheelEvent;

class Tool {
  public:
    DECLARE_BIG3_VIRTUAL (Tool, ViewMainWindow&, const glm::ivec2&, const QString&)

    Tool (ViewMainWindow&, const glm::ivec2&);

    friend class ToolUtil;

    ViewMainWindow&    mainWindow         ();
    const glm::ivec2&  clickPosition      () const;
    void               updateGlWidget     ();
    void               unselectAll        ();
    void               selectIntersection ();
    void               selectIntersection (const glm::ivec2&);

    void               render             ();
    void               mouseMoveEvent     (QMouseEvent&);
    void               mousePressEvent    (QMouseEvent&);
    void               mouseReleaseEvent  (QMouseEvent&);
    void               wheelEvent         (QWheelEvent&);
    QString            message            () const;

  protected:
    ViewToolOptions*   toolOptions        ();

  private:
    class Impl;
    Impl* impl;

    virtual void    runRender            ()             {}
    virtual bool    runMouseMoveEvent    (QMouseEvent&) { return false; }
    virtual bool    runMousePressEvent   (QMouseEvent&) { return false; }
    virtual bool    runMouseReleaseEvent (QMouseEvent&) { return false; }
    virtual bool    runWheelEvent        (QWheelEvent&) { return false; }
    virtual QString runMessage           () const       { return QString (); }
};

#endif
