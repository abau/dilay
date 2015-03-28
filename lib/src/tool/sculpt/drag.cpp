#include <glm/glm.hpp>
#include "cache.hpp"
#include "tools.hpp"
#include "sculpt-brush.hpp"
#include "state.hpp"
#include "tool/util/movement.hpp"
#include "view/double-slider.hpp"
#include "view/properties.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"

struct ToolSculptDrag::Impl {
  ToolSculptDrag*  self;
  ToolUtilMovement movement;

  Impl (ToolSculptDrag* s) 
    : self (s)
    , movement ( this->self->state ().camera ()
               , glm::vec3 (0.0f)
               , MovementConstraint::Explicit )
  {}

  void runSetupBrush (SculptBrush& brush) {
    auto& params = brush.parameters <SBMoveDirectionalParameters> ();

    params.smoothness       (this->self->cache ().get <float> ("smoothness", 0.5f));
    params.useLastPosition  (true);
    params.discardBackfaces (false);
  }

  void runSetupCursor (ViewCursor&) {}

  void runSetupProperties (ViewPropertiesPart& properties) {
    auto& params = this->self->brush ().parameters <SBMoveDirectionalParameters> ();

    ViewDoubleSlider& smoothnessEdit = ViewUtil::slider ( 0.0f, params.smoothness ()
                                                        , 1.0f, 0.1f );
    ViewUtil::connect (smoothnessEdit, [this,&params] (float f) {
      params.smoothness (f);
      this->self->cache ().set ("smoothness", f);
    });
    properties.addStacked (QObject::tr ("Smoothness"), smoothnessEdit);
  }

  void runSetupToolTip (ViewToolTip& toolTip) {
    toolTip.add (ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to sculpt"));
  }

  void runSculptMouseMoveEvent (const QMouseEvent& e) {
    this->self->draglikeStroke (e, this->movement);
  }

  bool runSculptMousePressEvent (const QMouseEvent& e) {
    return this->self->initializeDraglikeStroke (e, this->movement);
  }
};

DELEGATE_TOOL_SCULPT (ToolSculptDrag)
