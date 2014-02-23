#ifndef DILAY_TOOL
#define DILAY_TOOL

#include "macro.hpp"

class QMouseEvent;
class QContextMenuEvent;
class ViewMainWindow;

enum class ToolResponse { None, Terminate, Redraw };

class Tool {
  public:
    //DECLARE_BIG6_VIRTUAL (Tool)

    void initialize (ViewMainWindow* w, QContextMenuEvent* e) { 
      return this->runInitialize (w, e); 
    }

    void         render          ()               { return this->runRender          ( ); }
    ToolResponse mouseMoveEvent  (QMouseEvent* e) { return this->runMouseMoveEvent  (e); }
    ToolResponse mousePressEvent (QMouseEvent* e) { return this->runMousePressEvent (e); }

  private:
    /*
    class Impl;
    Impl* impl;
    */

    virtual void runInitialize (ViewMainWindow*, QContextMenuEvent*) {}

    virtual void         runRender          ()                {}
    virtual ToolResponse runMouseMoveEvent  (QMouseEvent*)    { return ToolResponse::None; }
    virtual ToolResponse runMousePressEvent (QMouseEvent*)    { return ToolResponse::None; }
};

#endif
