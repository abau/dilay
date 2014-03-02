#ifndef DILAY_TOOL
#define DILAY_TOOL

#include "macro.hpp"

class QMouseEvent;
class QContextMenuEvent;
class ViewMainWindow;
class ToolOptions;

class Tool {
  public:
    DECLARE_BIG3_VIRTUAL (Tool)

    ViewMainWindow* mainWindow      ();
    ToolOptions*    toolOptions     ();
    void            initialize      (ViewMainWindow*, QContextMenuEvent*);
    void            render          ();
    void            mouseMoveEvent  (QMouseEvent*);
    void            mousePressEvent (QMouseEvent*);
    void            close           ();

  private:
    class Impl;
    Impl* impl;

    virtual void runInitialize      (QContextMenuEvent*) {}
    virtual void runRender          ()                   {}
    virtual void runMouseMoveEvent  (QMouseEvent*)       {}
    virtual void runMousePressEvent (QMouseEvent*)       {}
    virtual void runApply           ()                   = 0;
};

#endif
