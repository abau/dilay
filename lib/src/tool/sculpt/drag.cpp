#include <QCheckBox>
#include <glm/glm.hpp>
#include "cache.hpp"
#include "sculpt-brush.hpp"
#include "state.hpp"
#include "tool/util/movement.hpp"
#include "tools.hpp"
#include "view/double-slider.hpp"
#include "view/properties.hpp"
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
    auto& params = brush.parameters <SBDraglikeParameters> ();

    params.smoothness       (this->self->cache ().get <float> ("smoothness", 0.5f));
    params.linearStep       (false);
    params.discardBackfaces (this->self->cache ().get <bool>  ("discard-backfaces", false));
  }

  void runSetupCursor (ViewCursor&) {}

  void runSetupProperties (ViewPropertiesPart& properties) {
    auto& params = this->self->brush ().parameters <SBDraglikeParameters> ();

    ViewDoubleSlider& smoothnessEdit = ViewUtil::slider ( 0.0f, params.smoothness ()
                                                        , 1.0f, 0.1f );
    ViewUtil::connect (smoothnessEdit, [this,&params] (float f) {
      params.smoothness (f);
      this->self->cache ().set ("smoothness", f);
    });
    properties.addStacked (QObject::tr ("Smoothness"), smoothnessEdit);

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

DELEGATE_TOOL_SCULPT (ToolSculptDrag)
