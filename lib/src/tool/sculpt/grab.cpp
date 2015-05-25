#include <QCheckBox>
#include <QMouseEvent>
#include <glm/glm.hpp>
#include "cache.hpp"
#include "sculpt-brush.hpp"
#include "state.hpp"
#include "tool/util/movement.hpp"
#include "tools.hpp"
#include "view/properties.hpp"
#include "view/util.hpp"

struct ToolSculptGrab::Impl {
  ToolSculptGrab*  self;
  ToolUtilMovement movement;

  Impl (ToolSculptGrab* s) 
    : self (s)
    , movement ( this->self->state ().camera ()
               , glm::vec3 (0.0f)
               , MovementConstraint::Explicit )
  {}

  void runSetupBrush (SculptBrush& brush) {
    auto& params = brush.parameters <SBDraglikeParameters> ();

    params.smoothness       (1.0f);
    params.linearStep       (true);
    params.discardBackfaces (this->self->cache ().get <bool>  ("discard-backfaces", false));
  }

  void runSetupCursor (ViewCursor&) {}

  void runSetupProperties (ViewPropertiesPart& properties) {
    auto& params = this->self->brush ().parameters <SBDraglikeParameters> ();
  
    QCheckBox& discardEdit = ViewUtil::checkBox ( QObject::tr ("Discard backfaces")
                                                , params.discardBackfaces () );
    ViewUtil::connect (discardEdit, [this,&params] (bool d) {
      params.discardBackfaces (d);
      this->self->cache ().set ("discard-backfaces", d);
    });
    properties.add (discardEdit);
  }

  void runSetupToolTip (ViewToolTip& toolTip) {
    this->self->addDefaultToolTip (toolTip, false);
  }

  void runSculptMouseMoveEvent (const QMouseEvent& e) {
    this->self->draglikeStroke (e, this->movement);
  }

  bool runSculptMousePressEvent (const QMouseEvent& e) {
    return this->self->initializeDraglikeStroke (e, this->movement);
  }
};

DELEGATE_TOOL_SCULPT (ToolSculptGrab)
