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

    ViewMainWindow*    mainWindow        ();
    QContextMenuEvent* menuEvent         ();
    ViewToolOptions*   toolOptions       ();
    void               render            ();
    void               mouseMoveEvent    (QMouseEvent*);
    void               mousePressEvent   (QMouseEvent*);
    void               mouseReleaseEvent (QMouseEvent*);

  protected:
    Tool ();

  private:
    class Impl;
    Impl* impl;

    virtual void runRender            ()             {}
    virtual bool runMouseMoveEvent    (QMouseEvent*) { return false; }
    virtual bool runMousePressEvent   (QMouseEvent*) { return false; }
    virtual bool runMouseReleaseEvent (QMouseEvent*) { return false; }
};

#define DECLARE_TOOL(t,...)                                              \
  DECLARE_BIG3 ( t , ViewMainWindow*, QContextMenuEvent*, ##__VA_ARGS__) \
  static QString toolName ();

#define DELEGATE_TOOL_WITHOUT_CONSTRUCTOR(t)          \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF ( t )        \
  DELEGATE_STATIC (QString, t , toolName)

#define DELEGATE_TOOL(t)                              \
  t :: t (ViewMainWindow* mW, QContextMenuEvent* mE)  \
    : Tool (mW, mE, t :: toolName ()) {               \
        DELEGATE_NEW_IMPL (this)                      \
    }                                                 \
  DELEGATE_TOOL_WITHOUT_CONSTRUCTOR( t )

#define DELEGATE1_TOOL(t,t1)                          \
  t :: t ( ViewMainWindow* mW, QContextMenuEvent* mE  \
         , t1 a1, t2 a2 )                             \
    : Tool (mW, mE, t :: toolName ()) {               \
        DELEGATE_NEW_IMPL (this,a1,a2)                \
    }                                                 \
  DELEGATE_TOOL_WITHOUT_CONSTRUCTOR( t )

#define DELEGATE2_TOOL(t,t1,t2)                       \
  t :: t ( ViewMainWindow* mW, QContextMenuEvent* mE  \
         , t1 a1, t2 a2 )                             \
    : Tool (mW, mE, t :: toolName ()) {               \
        DELEGATE_NEW_IMPL (this,a1,a2)                \
    }                                                 \
  DELEGATE_TOOL_WITHOUT_CONSTRUCTOR( t )

#define DELEGATE3_TOOL(t,t1,t2,t3)                    \
  t :: t ( ViewMainWindow* mW, QContextMenuEvent* mE  \
         , t1 a1, t2 a2, t3 a3 )                      \
    : Tool (mW, mE, t :: toolName ()) {               \
        DELEGATE_NEW_IMPL (this,a1,a2,a3)             \
    }                                                 \
  DELEGATE_TOOL_WITHOUT_CONSTRUCTOR( t )

#define DELEGATE4_TOOL(t,t1,t2,t3,t4)                 \
  t :: t ( ViewMainWindow* mW, QContextMenuEvent* mE  \
         , t1 a1, t2 a2, t3 a3, t4 a4 )               \
    : Tool (mW, mE, t :: toolName ()) {               \
        DELEGATE_NEW_IMPL (this,a1,a2,a3,a4)          \
    }                                                 \
  DELEGATE_TOOL_WITHOUT_CONSTRUCTOR( t )

#define DELEGATE5_TOOL(t,t1,t2,t3,t4,t5)              \
  t :: t ( ViewMainWindow* mW, QContextMenuEvent* mE  \
         , t1 a1, t2 a2, t3 a3, t4 a4, t5 a5 )        \
    : Tool (mW, mE, t :: toolName ()) {               \
        DELEGATE_NEW_IMPL (this,a1,a2,a3,a4,a5)       \
    }                                                 \
  DELEGATE_TOOL_WITHOUT_CONSTRUCTOR( t )

#endif
