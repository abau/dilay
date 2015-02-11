#include <QDoubleSpinBox>
#include <glm/glm.hpp>
#include "config.hpp"
#include "sculpt-brush/carve.hpp"
#include "tool/sculpt/behaviors.hpp"
#include "view/cursor.hpp"
#include "view/properties.hpp"
#include "view/tool/tip.hpp"
#include "view/util.hpp"
#include "winged/face-intersection.hpp"

struct ToolSculptCarve::Impl {
  ToolSculptCarve* self;
  SculptBrushCarve brush;

  Impl (ToolSculptCarve* s) 
    : self (s) 
  {
    this->brush.intensityFactor (this->self->config ().get <float> ("intensity-factor" , 0.1f));
  }

  void runSetupProperties (ViewProperties& properties) {
    QDoubleSpinBox& intensityEdit = ViewUtil::spinBox ( 0.0f, this->brush.intensityFactor ()
                                                      , 1000.0f, 0.1f );
    ViewUtil::connect (intensityEdit, [this] (float d) {
      this->brush.intensityFactor (d);
      this->self->config ().cache ("intensity", d);
    });
    properties.addWidget (QObject::tr ("Intensity"), intensityEdit);
  }

  void runSetupToolTip (ViewToolTip& toolTip) {
    toolTip.add (ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to carve"));
  }

  void runMouseLeftPressEvent (const glm::ivec2& pos) {
    this->runMouseMoveEvent (pos, true);
  }

  void runMouseMoveEvent (const glm::ivec2& pos, bool leftButton) {
    WingedFaceIntersection intersection;

    if (this->self->intersectsSelection (pos, intersection)) {
      this->self->cursor ().position (intersection.position ());
      this->self->cursor ().normal   (intersection.normal   ());

      if (leftButton) {
        this->brush.mesh (&intersection.mesh ());
        this->brush.face (&intersection.face ());

        if (this->brush.updatePosition (intersection.position ())) {
          this->self->sculpt ();
        }
      }
    }
  }
};

DELEGATE_TOOL_SCULPT_BEHAVIOR (ToolSculptCarve)
