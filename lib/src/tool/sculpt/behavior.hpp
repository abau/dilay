#ifndef DILAY_TOOL_SCULPT_BEHAVIOR
#define DILAY_TOOL_SCULPT_BEHAVIOR

#include <glm/fwd.hpp>
#include "macro.hpp"

class ConfigProxy;
class QDoubleSpinBox;
class SculptBrush;
class State;
class ViewCursor;
class ViewProperties;
class ViewToolTip;
class WingedFaceIntersection;

class ToolSculptBehavior {
  public:
    DECLARE_BIG3_VIRTUAL (ToolSculptBehavior, ConfigProxy&, State&)

    virtual SculptBrush&    brush      () const = 0;
            ViewCursor&     cursor     () const;
            QDoubleSpinBox& radiusEdit () const;

    void setupBrush          ();
    void setupProperties     (ViewProperties&);
    void setupToolTip        (ViewToolTip&);
    void mouseMoveEvent      (const glm::ivec2&, bool);
    void mouseLeftPressEvent (const glm::ivec2&);
    void addActionsToHistory ();

  protected:
    ConfigProxy& config              () const;
    State&       state               () const;
    bool         intersectsSelection (const glm::ivec2&, WingedFaceIntersection&) const;
    void         sculpt              ();

  private:
    IMPLEMENTATION

    virtual void runSetupBrush          () = 0;
    virtual void runSetupProperties     (ViewProperties&) = 0;
    virtual void runSetupToolTip        (ViewToolTip&) = 0;
    virtual void runMouseLeftPressEvent (const glm::ivec2&) = 0;
    virtual void runMouseMoveEvent      (const glm::ivec2&, bool) = 0;
};

#define DECLARE_TOOL_BEHAVIOR(name)                                                         \
  class name : public ToolSculptBehavior {                                                  \
    public:  DECLARE_BIG3 (name, ConfigProxy&, State&)                                      \
             SculptBrush& brush () const;                                                   \
    private: IMPLEMENTATION                                                                 \
             void runSetupBrush          ();                                                \
             void runSetupProperties     (ViewProperties&);                                 \
             void runSetupToolTip        (ViewToolTip&);                                    \
             void runMouseLeftPressEvent (const glm::ivec2&);                               \
             void runMouseMoveEvent      (const glm::ivec2&, bool);                         \
  };

#define DELEGATE_TOOL_BEHAVIOR(name)                                                        \
  DELEGATE_BIG3_BASE (name, (ConfigProxy& c, State& s), (this), ToolSculptBehavior, (c,s))  \
  GETTER_CONST (SculptBrush&, name, brush)                                                  \
  DELEGATE     (void, name, runSetupBrush)                                                  \
  DELEGATE1    (void, name, runSetupProperties, ViewProperties&);                           \
  DELEGATE1    (void, name, runSetupToolTip, ViewToolTip&);                                 \
  DELEGATE1    (void, name, runMouseLeftPressEvent, const glm::ivec2&)                      \
  DELEGATE2    (void, name, runMouseMoveEvent, const glm::ivec2&, bool)

#endif
