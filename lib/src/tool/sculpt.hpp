#ifndef DILAY_TOOL_SCULPT
#define DILAY_TOOL_SCULPT

#include "tool.hpp"

class SculptBrush;
class ViewCursor;
class ViewPropertiesPart;

class ToolSculpt : public Tool {
  public:
    DECLARE_BIG2 (ToolSculpt, State&, const char*)

  protected:
    void sculpt                             ();
    void updateCursorByIntersection         (const QMouseEvent&);
    bool updateBrushAndCursorByIntersection (const QMouseEvent&);

  private:
    IMPLEMENTATION

    bool         runAllowUndoRedo     () const;
    ToolResponse runInitialize        ();
    void         runRender            () const;
    ToolResponse runMouseReleaseEvent (const QMouseEvent&);
    ToolResponse runWheelEvent        (const QWheelEvent&);
    void         runClose             ();

    virtual const char*  key                  () const = 0;
    virtual SculptBrush& brush                () const = 0;
    virtual ViewCursor&  cursor               () const = 0;
    virtual void         runSetupBrush        () = 0;
    virtual void         runSetupCursor       () = 0;
    virtual void         runSetupProperties   (ViewPropertiesPart&) = 0;
    virtual void         runSetupToolTip      (ViewToolTip&) = 0;
    virtual ToolResponse runMouseMoveEvent    (const QMouseEvent&) = 0;
    virtual ToolResponse runMousePressEvent   (const QMouseEvent&) = 0;
};

#define DECLARE_TOOL_SCULPT(name,theKey)                                             \
  class name : public ToolSculpt { public:                                           \
    DECLARE_BIG2 (name, State&)                                                      \
    private:                                                                         \
      IMPLEMENTATION                                                                 \
      const char* key                     () const { return theKey ; }               \
      SculptBrush& brush                  () const;                                  \
      ViewCursor&  cursor                 () const;                                  \
      void         runSetupBrush          ();                                        \
      void         runSetupCursor         ();                                        \
      void         runSetupProperties     (ViewPropertiesPart&);                     \
      void         runSetupToolTip        (ViewToolTip&);                            \
      ToolResponse runMouseMoveEvent      (const QMouseEvent&);                      \
      ToolResponse runMousePressEvent     (const QMouseEvent&);                      \
  };

#define DELEGATE_TOOL_SCULPT(name)                                                   \
  DELEGATE_BIG2_BASE ( name, (State& s), (this)                                      \
                     , ToolSculpt, (s,this->key ()) )                                \
  GETTER_CONST   (SculptBrush&, name, brush)                                         \
  GETTER_CONST   (ViewCursor& , name, cursor)                                        \
  DELEGATE       (void        , name, runSetupBrush);                                \
  DELEGATE       (void        , name, runSetupCursor);                               \
  DELEGATE1      (void        , name, runSetupProperties, ViewPropertiesPart&);      \
  DELEGATE1      (void        , name, runSetupToolTip, ViewToolTip&);                \
  DELEGATE1      (ToolResponse, name, runMouseMoveEvent, const QMouseEvent&)         \
  DELEGATE1      (ToolResponse, name, runMousePressEvent, const QMouseEvent&)

#endif
