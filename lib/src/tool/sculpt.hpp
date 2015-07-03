/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL_SCULPT
#define DILAY_TOOL_SCULPT

#include "tool.hpp"

class SculptBrush;
class ToolUtilMovement;
class ViewCursor;
class ViewPropertiesPart;

class ToolSculpt : public Tool {
  public:
    DECLARE_BIG2 (ToolSculpt, State&, const char*)

  protected:
    SculptBrush& brush                    ();
    ViewCursor&  cursor                   ();
    void         addDefaultToolTip        (ViewToolTip&, bool) const;
    void         sculpt                   ();
    bool         carvelikeStroke          ( const QMouseEvent&, bool
                                          , const std::function <void ()>* = nullptr );
    bool         initializeDraglikeStroke (const QMouseEvent&, ToolUtilMovement&);
    bool         draglikeStroke           (const QMouseEvent&, ToolUtilMovement&);

  private:
    IMPLEMENTATION

    ToolResponse runInitialize        ();
    void         runRender            () const;
    ToolResponse runMouseMoveEvent    (const QMouseEvent&);
    ToolResponse runMousePressEvent   (const QMouseEvent&);
    ToolResponse runMouseReleaseEvent (const QMouseEvent&);
    ToolResponse runWheelEvent        (const QWheelEvent&);
    void         runClose             ();

    virtual const char* key                      () const = 0;
    virtual void        runSetupBrush            (SculptBrush&) = 0;
    virtual void        runSetupCursor           (ViewCursor&) = 0;
    virtual void        runSetupProperties       (ViewPropertiesPart&) = 0;
    virtual void        runSetupToolTip          (ViewToolTip&) = 0;
    virtual void        runSculptMouseMoveEvent  (const QMouseEvent&) = 0;
    virtual bool        runSculptMousePressEvent (const QMouseEvent&) = 0;
};

#define DECLARE_TOOL_SCULPT(name,theKey)                                             \
  class name : public ToolSculpt { public:                                           \
    DECLARE_BIG2 (name, State&)                                                      \
    private:                                                                         \
      IMPLEMENTATION                                                                 \
      const char* key                () const { return theKey ; }                    \
      void        runSetupBrush      (SculptBrush&);                                 \
      void        runSetupCursor     (ViewCursor&);                                  \
      void        runSetupProperties (ViewPropertiesPart&);                          \
      void        runSetupToolTip    (ViewToolTip&);                                 \
      void        runSculptMouseMoveEvent  (const QMouseEvent&);                     \
      bool        runSculptMousePressEvent (const QMouseEvent&);                     \
  };

#define DELEGATE_TOOL_SCULPT(name)                                                   \
  DELEGATE_BIG2_BASE ( name, (State& s), (this)                                      \
                     , ToolSculpt, (s,this->key ()) )                                \
  DELEGATE1 (void, name, runSetupBrush, SculptBrush&);                               \
  DELEGATE1 (void, name, runSetupCursor, ViewCursor&);                               \
  DELEGATE1 (void, name, runSetupProperties, ViewPropertiesPart&);                   \
  DELEGATE1 (void, name, runSetupToolTip, ViewToolTip&);                             \
  DELEGATE1 (void, name, runSculptMouseMoveEvent, const QMouseEvent&)                \
  DELEGATE1 (bool, name, runSculptMousePressEvent, const QMouseEvent&)

#endif
