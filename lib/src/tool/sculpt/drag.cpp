#include <QDoubleSpinBox>
#include <glm/glm.hpp>
#include "cache.hpp"
#include "tools.hpp"
#include "sculpt-brush.hpp"
#include "state.hpp"
#include "tool/util/movement.hpp"
#include "view/cursor.hpp"
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
    brush.innerRadiusFactor (this->self->cache ().get <float> ("inner-radius-factor", 0.5f));
    brush.useLastPosition   (true);
    brush.useIntersection   (true);
  }

  void runSetupCursor (ViewCursor& cursor) {
    cursor.hasInnerRadius    (true);
    cursor.innerRadiusFactor (this->self->brush ().innerRadiusFactor ());
  }

  void runSetupProperties (ViewPropertiesPart& properties) {
    SculptBrush& brush = this->self->brush ();

    QDoubleSpinBox& innerRadiusEdit = ViewUtil::spinBox ( 0.0f, brush.innerRadiusFactor ()
                                                        , 1.0f, 0.1f );
    ViewUtil::connect (innerRadiusEdit, [this,&brush] (float f) {
      brush.innerRadiusFactor (f);
      this->self->cursor ().innerRadiusFactor (f);
      this->self->cache ().set ("inner-radius-factor", f);
    });
    properties.add (QObject::tr ("Inner radius"), innerRadiusEdit);
  }

  void runSetupToolTip (ViewToolTip& toolTip) {
    toolTip.add (ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to sculpt"));
  }

  ToolResponse runMouseMoveEvent (const QMouseEvent& e) {
    this->self->draglikeStroke (e, this->movement);
    return ToolResponse::Redraw;
  }

  ToolResponse runMousePressEvent (const QMouseEvent& e) {
    this->self->initializeDraglikeStroke (e, this->movement);
    return ToolResponse::Redraw;
  }
};

DELEGATE_TOOL_SCULPT (ToolSculptDrag)
