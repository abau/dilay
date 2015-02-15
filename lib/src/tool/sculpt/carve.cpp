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
    this->brush.radius          (20.0f);
    this->brush.detailFactor    (0.7f);
    this->brush.stepWidthFactor (0.3f);
    this->brush.subdivide       (true);

    this->self->brushFromCache  (this->brush);
    this->brush.intensityFactor (this->self->config ().get <float> ("intensity-factor" , 0.02f));
    this->brush.flatness        (this->self->config ().get <int>   ("flatness"         , 50));
  }

  void runSetupProperties (ViewPropertiesPart& properties) {
    QDoubleSpinBox& intensityEdit = ViewUtil::spinBox ( 0.0f, this->brush.intensityFactor ()
                                                      , 0.1f, 0.01f );
    ViewUtil::connect (intensityEdit, [this] (float i) {
      this->brush.intensityFactor (i);
      this->self->config ().cache ("intensity", i);
    });
    properties.add (QObject::tr ("Intensity"), intensityEdit);

    QSpinBox& flatnessEdit = ViewUtil::spinBox (3, this->brush.flatness (), 1000);
    ViewUtil::connect (flatnessEdit, [this] (int f) {
      this->brush.flatness (f);
      this->self->config ().cache ("flatness", f);
    });
    properties.add (QObject::tr ("Flatness"), flatnessEdit);
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
