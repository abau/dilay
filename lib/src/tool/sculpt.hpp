/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_TOOL_SCULPT
#define DILAY_TOOL_SCULPT

#include "tool.hpp"

class QPushButton;
class QString;
class SculptBrush;
class ToolUtilMovement;
class ViewCursor;
class ViewDoubleSlider;
class ViewTwoColumnGrid;

class ToolSculpt : public Tool
{
public:
  DECLARE_BIG2 (ToolSculpt, State&, QPushButton&, const char*)

protected:
  SculptBrush& brush ();
  ViewCursor&  cursor ();
  void         addDefaultToolTip (ViewToolTip&, bool) const;
  void         addSecSliderWheelToolTip (ViewToolTip&, const QString&) const;
  void         sculpt ();
  bool drawlikeStroke (const ViewPointingEvent&, bool, const std::function<void()>* = nullptr);
  bool grablikeStroke (const ViewPointingEvent&, ToolUtilMovement&);
  void registerSecondarySlider (ViewDoubleSlider&);

private:
  IMPLEMENTATION

  ToolResponse runInitialize ();
  void         runRender () const;
  ToolResponse runPointingEvent (const ViewPointingEvent&);
  ToolResponse runWheelEvent (const QWheelEvent&);
  ToolResponse runCursorUpdate (const glm::ivec2&);
  void         runFromConfig ();

  virtual const char* key () const = 0;
  virtual void        runSetupBrush (SculptBrush&) = 0;
  virtual void        runSetupCursor (ViewCursor&) = 0;
  virtual void        runSetupProperties (ViewTwoColumnGrid&) = 0;
  virtual void        runSetupToolTip (ViewToolTip&) = 0;
  virtual bool        runSculptPointingEvent (const ViewPointingEvent&) = 0;
};

#define DECLARE_TOOL_SCULPT(name, theKey)                   \
  class name : public ToolSculpt                            \
  {                                                         \
  public:                                                   \
    DECLARE_BIG2 (name, State&, QPushButton&)               \
  private:                                                  \
    IMPLEMENTATION                                          \
    const char* key () const                                \
    {                                                       \
      return theKey;                                        \
    }                                                       \
    void runSetupBrush (SculptBrush&);                      \
    void runSetupCursor (ViewCursor&);                      \
    void runSetupProperties (ViewTwoColumnGrid&);           \
    void runSetupToolTip (ViewToolTip&);                    \
    bool runSculptPointingEvent (const ViewPointingEvent&); \
  };

#define DELEGATE_TOOL_SCULPT(name)                                            \
  DELEGATE_BIG2_BASE (name, (State & s, QPushButton & b), (this), ToolSculpt, \
                      (s, b, this->key ()))                                   \
  DELEGATE1 (void, name, runSetupBrush, SculptBrush&);                        \
  DELEGATE1 (void, name, runSetupCursor, ViewCursor&);                        \
  DELEGATE1 (void, name, runSetupProperties, ViewTwoColumnGrid&);             \
  DELEGATE1 (void, name, runSetupToolTip, ViewToolTip&);                      \
  DELEGATE1 (bool, name, runSculptPointingEvent, const ViewPointingEvent&)

#endif
