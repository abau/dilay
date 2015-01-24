#include <QDoubleSpinBox>
#include <glm/glm.hpp>
#include "camera.hpp"
#include "config.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
#include "sculpt-brush/carve.hpp"
#include "selection.hpp"
#include "state.hpp"
#include "tool/sculpt/behaviors.hpp"
#include "view/cursor.hpp"
#include "view/properties.hpp"
#include "view/tool/tip.hpp"
#include "view/util.hpp"
#include "winged/face-intersection.hpp"
#include "winged/mesh.hpp"

struct ToolSculptCarve::Impl {
  ToolSculptCarve* self;
  SculptBrushCarve brush;

  Impl (ToolSculptCarve* s) : self (s) {}

  void runSetupBrush () {
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

  bool runMouseMoveEvent (State& state, const glm::ivec2& pos, bool leftButton) {
    const PrimRay          ray = state.camera ().ray (pos);
    WingedFaceIntersection intersection;

    if (   state.scene ().intersects (ray, intersection) 
        && state.scene ().selection  ().hasMajor (intersection.mesh ().index ())) 
    {
      this->self->cursor ().position (intersection.position ());
      this->self->cursor ().normal   (intersection.normal   ());

      if (leftButton) {
        this->brush.mesh (&intersection.mesh ());
        this->brush.face (&intersection.face ());

        return this->brush.updatePosition (intersection.position ());
      }
      else {
        return false;
      }
    }
    else {
      return false;
    }
  }
};

DELEGATE_TOOL_BEHAVIOR (ToolSculptCarve)
