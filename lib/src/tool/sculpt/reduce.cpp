#include "cache.hpp"
#include "scene.hpp"
#include "sculpt-brush.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "view/double-slider.hpp"
#include "view/properties.hpp"
#include "view/util.hpp"

struct ToolSculptReduce::Impl {
  ToolSculptReduce* self;

  Impl (ToolSculptReduce* s) : self (s) {}

  void runSetupBrush (SculptBrush& brush) {
    auto& params = brush.parameters <SBReduceParameters> ();

    params.intensity (this->self->cache ().get <float> ("intensity", 0.75f));
  }

  void runSetupCursor (ViewCursor&) {}

  void runSetupProperties (ViewPropertiesPart& properties) {
    auto& params = this->self->brush ().parameters <SBReduceParameters> ();

    ViewDoubleSlider& intensityEdit = ViewUtil::slider ( 0.1f, params.intensity ()
                                                       , 0.9f, 0.05f );
    ViewUtil::connect (intensityEdit, [this,&params] (float i) {
      params.intensity (i);
      this->self->cache ().set ("intensity", i);
    });
    properties.addStacked (QObject::tr ("Intensity"), intensityEdit);
  }

  void runSetupToolTip (ViewToolTip& toolTip) {
    this->self->addDefaultToolTip (toolTip, false);
  }

  bool runMouseEvent (const QMouseEvent& e) {
    const bool result = this->self->carvelikeStroke (e, false);
    this->self->state ().scene ().deleteEmptyMeshes ();
    return result;
  }

  void runSculptMouseMoveEvent (const QMouseEvent& e) {
    this->runMouseEvent (e);
  }

  bool runSculptMousePressEvent (const QMouseEvent& e) {
    return this->runMouseEvent (e);
  }
};

DELEGATE_TOOL_SCULPT (ToolSculptReduce)
