#ifndef DILAY_TOOL
#define DILAY_TOOL

#include "macro.hpp"

class QMouseEvent;
class QContextMenuEvent;
class ViewMainWindow;
class ViewToolOptions;
class QString;
class QWheelEvent;

class Tool {
  public:
    DECLARE_BIG3_VIRTUAL (Tool, ViewMainWindow*, QContextMenuEvent*, const QString&)

    friend class ToolUtil;

    ViewMainWindow*    mainWindow        ();
    QContextMenuEvent* menuEvent         ();
    void               updateGlWidget    ();

    void               render            ();
    void               mouseMoveEvent    (QMouseEvent*);
    void               mousePressEvent   (QMouseEvent*);
    void               mouseReleaseEvent (QMouseEvent*);
    void               wheelEvent        (QWheelEvent*);

  protected:
    Tool ();

    ViewToolOptions*   toolOptions       ();

    bool               isDraged          () const;
    void               drag              (bool);
    void               dragIfHovered     ();
    bool               isHovered         () const;
    void               hover             (bool);
    void               hoverIfDraged     ();

  private:
    class Impl;
    Impl* impl;

    virtual void runRender            ()             {}
    virtual bool runMouseMoveEvent    (QMouseEvent*) { return false; }
    virtual bool runMousePressEvent   (QMouseEvent*) { return false; }
    virtual bool runMouseReleaseEvent (QMouseEvent*) { return false; }
    virtual bool runWheelEvent        (QWheelEvent*) { return false; }
};

#define DECLARE_TOOL(t,...)                                              \
  DECLARE_BIG3 ( t , ViewMainWindow*, QContextMenuEvent*, ##__VA_ARGS__) \
  static QString toolName ( __VA_ARGS__ );

#define DELEGATE_TOOL(t)                                      \
  t :: t (ViewMainWindow* mW, QContextMenuEvent* mE)          \
    : Tool (mW, mE, t :: toolName ()) {                       \
        DELEGATE_NEW_IMPL (this)                              \
    }                                                         \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF ( t )                \
  DELEGATE_STATIC (QString, t , toolName)

#define DELEGATE1_TOOL(t,t1)                                  \
  t :: t ( ViewMainWindow* mW, QContextMenuEvent* mE          \
         , t1 a1 )                                            \
    : Tool (mW, mE, t :: toolName (a1)) {                     \
        DELEGATE_NEW_IMPL (this,a1)                           \
    }                                                         \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF ( t )                \
  DELEGATE1_STATIC (QString, t, toolName, t1)

#define DELEGATE2_TOOL(t,t1,t2)                               \
  t :: t ( ViewMainWindow* mW, QContextMenuEvent* mE          \
         , t1 a1, t2 a2 )                                     \
    : Tool (mW, mE, t :: toolName (a1,a2)) {                  \
        DELEGATE_NEW_IMPL (this,a1,a2)                        \
    }                                                         \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF ( t )                \
  DELEGATE2_STATIC (QString, t, toolName, t1, t2)

#define DELEGATE3_TOOL(t,t1,t2,t3)                            \
  t :: t ( ViewMainWindow* mW, QContextMenuEvent* mE          \
         , t1 a1, t2 a2, t3 a3 )                              \
    : Tool (mW, mE, t :: toolName (a1,a2,a3)) {               \
        DELEGATE_NEW_IMPL (this,a1,a2,a3)                     \
    }                                                         \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF ( t )                \
  DELEGATE3_STATIC (QString, t, toolName, t1, t2, t3)

#define DELEGATE4_TOOL(t,t1,t2,t3,t4)                         \
  t :: t ( ViewMainWindow* mW, QContextMenuEvent* mE          \
         , t1 a1, t2 a2, t3 a3, t4 a4 )                       \
    : Tool (mW, mE, t :: toolName (a1,a2,a3,a4)) {            \
        DELEGATE_NEW_IMPL (this,a1,a2,a3,a4)                  \
    }                                                         \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF ( t )                \
  DELEGATE4_STATIC (QString, t, toolName, t1, t2, t3, t4)

#define DELEGATE5_TOOL(t,t1,t2,t3,t4,t5)                      \
  t :: t ( ViewMainWindow* mW, QContextMenuEvent* mE          \
         , t1 a1, t2 a2, t3 a3, t4 a4, t5 a5 )                \
    : Tool (mW, mE, t :: toolName (a1,a2,a3,a4,a5)) {         \
        DELEGATE_NEW_IMPL (this,a1,a2,a3,a4,a5)               \
    }                                                         \
  DELEGATE_BIG3_WITHOUT_CONSTRUCTOR_SELF ( t )                \
  DELEGATE5_STATIC (QString, t, toolName, t1, t2, t3, t4, t5)

#endif
