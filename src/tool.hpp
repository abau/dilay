#ifndef DILAY_TOOL
#define DILAY_TOOL

#include "macro.hpp"

class QMouseEvent;
class QContextMenuEvent;
class ViewMainWindow;
class ViewToolOptions;
class QString;

class Tool {
  public:
    DECLARE_BIG3_VIRTUAL (Tool)

    ViewMainWindow*  mainWindow      ();
    ViewToolOptions* toolOptions     ();
    void             initialize      (ViewMainWindow*, QContextMenuEvent*, const QString&);
    void             render          ();
    void             mouseMoveEvent  (QMouseEvent*);
    void             mousePressEvent (QMouseEvent*);
    void             close           ();

  private:
    class Impl;
    Impl* impl;

    virtual void runInitialize      (QContextMenuEvent*) {}
    virtual void runRender          ()                   {}
    virtual void runMouseMoveEvent  (QMouseEvent*)       {}
    virtual void runMousePressEvent (QMouseEvent*)       {}
    virtual void runClose           ()                   {};
};

#endif
