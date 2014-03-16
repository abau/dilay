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
    DECLARE_BIG3_VIRTUAL (Tool, ViewMainWindow*, QContextMenuEvent*, const QString&)

    ViewMainWindow*    mainWindow      ();
    QContextMenuEvent* menuEvent       ();
    ViewToolOptions*   toolOptions     ();
    void               render          ();
    void               mouseMoveEvent  (QMouseEvent*);
    void               mousePressEvent (QMouseEvent*);

  protected:
    Tool ();

  private:
    class Impl;
    Impl* impl;

    virtual void runRender          ()             {}
    virtual bool runMouseMoveEvent  (QMouseEvent*) { return false; }
    virtual bool runMousePressEvent (QMouseEvent*) { return false; }
};

#define DECLARE_TOOL(t)                                   \
  DECLARE_BIG3 ( t , ViewMainWindow*, QContextMenuEvent*) \
  static QString toolName ();

#define DELEGATE_TOOL(t)                              \
  t :: t (ViewMainWindow* mW, QContextMenuEvent* mE)  \
    : Tool (mW, mE, t :: toolName ()) {               \
        DELEGATE_NEW_IMPL (this)                      \
    }                                                 \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF ( t )        \
  DELEGATE_STATIC (QString, t , toolName)

#endif
