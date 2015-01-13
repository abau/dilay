#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QMouseEvent>
#include <glm/glm.hpp>
#include "action/sculpt.hpp"
#include "action/unit.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "config.hpp"
#include "history.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
#include "sculpt-brush/carve.hpp"
#include "selection.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "view/cursor.hpp"
#include "view/properties.hpp"
#include "view/tool/tip.hpp"
#include "view/util.hpp"
#include "winged/face-intersection.hpp"
#include "winged/mesh.hpp"

struct ToolCarve::Impl {
  ToolCarve*                   self;
  std::unique_ptr <ActionUnit> actions;
  SculptBrushCarve             brush;
  ViewCursor                   cursor;
  QDoubleSpinBox*              radiusEdit;

  Impl (ToolCarve* s) 
    : self    (s) 
    , actions (new ActionUnit) 
    , cursor  (1.0f, this->self->config ().get <Color> ("cursor-color", Color::red ()))
  {
    this->brush.radius          (this->self->config ().get <float> ("radius"           , 10.0f));
    this->brush.detailFactor    (this->self->config ().get <float> ("detail-factor"    ,  0.6f));
    this->brush.intensityFactor (this->self->config ().get <float> ("intensity-factor" ,  0.1f));
    this->brush.stepWidthFactor (this->self->config ().get <float> ("step-width-factor",  0.3f));
    this->brush.subdivide       (this->self->config ().get <bool>  ("subdivide"        ,  true));

    this->setupProperties ();
    this->setupToolTip    ();

    this->cursor.radius         (this->brush.radius ());
    this->cursor.updateGeometry ();
    this->updateCursor          (this->self->cursorPosition (), false);
  }

  void setupProperties () {
    this->radiusEdit = &ViewUtil::spinBox (0.01f, this->brush.radius (), 1000.0f, 1.0f);
    ViewUtil::connect (*radiusEdit, [this] (float r) {
      this->brush .radius         (r);
      this->cursor.radius         (r);
      this->cursor.updateGeometry ();
      this->self->updateGlWidget  ();
      this->self->config ().cache ("radius", r);
    });
    this->self->properties ().addWidget (QObject::tr ("Radius"), *radiusEdit);

    QDoubleSpinBox& detailEdit = ViewUtil::spinBox (0.01f, this->brush.detailFactor (), 0.95f, 0.1f);
    ViewUtil::connect (detailEdit, [this] (float h) {
      this->brush.detailFactor (h);
      this->self->config ().cache ("detail-factor", h);
    });
    this->self->properties ().addWidget (QObject::tr ("Detail"), detailEdit);

    QDoubleSpinBox& intensityEdit = ViewUtil::spinBox ( 0.0f, this->brush.intensityFactor ()
                                                      , 1000.0f, 0.1f );
    ViewUtil::connect (intensityEdit, [this] (float d) {
      this->brush.intensityFactor (d);
      this->self->config ().cache ("intensity", d);
    });
    this->self->properties ().addWidget (QObject::tr ("Intensity"), intensityEdit);

    QDoubleSpinBox& stepEdit = ViewUtil::spinBox ( 0.01f, this->brush.stepWidthFactor ()
                                                 , 1000.0f, 0.1f );
    ViewUtil::connect (stepEdit, [this] (float s) {
      this->brush.stepWidthFactor (s);
      this->self->config ().cache ("step-width-factor", s);
    });
    this->self->properties ().addWidget (QObject::tr ("Step width"), stepEdit);

    QCheckBox& subdivEdit = ViewUtil::checkBox (QObject::tr ("Subdivide"), this->brush.subdivide ());
    QObject::connect (&subdivEdit, &QCheckBox::stateChanged, [this] (int s) {
      this->brush.subdivide (bool (s));
      this->self->config ().cache ("subdivide", bool (s));
    });
    this->self->properties ().addWidget (subdivEdit);
  }

  void setupToolTip () {
    this->self->resetToolTip ();
    this->self->toolTip ().add ( ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to carve"));
    this->self->toolTip ().add ( ViewToolTip::MouseEvent::Wheel, ViewToolTip::Modifier::Shift
                               , QObject::tr ("Change radius") );
    this->self->showToolTip ();
  }


  bool updateCursor (const glm::ivec2& mouse, bool updateBrush) {
    PrimRay                ray   = this->self->state ().camera ().ray (mouse);
    Scene&                 scene = this->self->state ().scene ();
    WingedFaceIntersection intersection;

    if (   scene.intersects (ray, intersection) 
        && scene.selection  ().hasMajor (intersection.mesh ().index ())) 
    {
      this->cursor.position (intersection.position ());
      this->cursor.normal   (intersection.normal   ());

      return updateBrush ? this->brush.update ( intersection.mesh     ()
                                              , intersection.face     ()
                                              , intersection.position () )
                         : false;
    }
    else {
      return false;
    }
  }

  void runRender () {
    this->cursor.render (this->self->state ().camera ());
  }

  ToolResponse runMouseMoveEvent (QMouseEvent& e) {
    const bool doCarve = e.buttons () == Qt::LeftButton;
    if (this->updateCursor (ViewUtil::toIVec2 (e), doCarve)) {
      this->actions->add <ActionSculpt, WingedMesh> 
        (this->self->state ().scene (), this->brush.mesh ()).run (this->brush);
    }
    return ToolResponse::Redraw;
  }

  ToolResponse runMouseReleaseEvent (QMouseEvent& e) {
    if (e.button () == Qt::LeftButton && this->actions->isEmpty () == false) {
      this->self->state ().history ().addUnit (std::move (*this->actions));
      this->actions.reset (new ActionUnit ());
    }
    return ToolResponse::None;
  }

  ToolResponse runWheelEvent (QWheelEvent& e) {
    if (e.orientation () == Qt::Vertical && e.modifiers ().testFlag (Qt::ShiftModifier)) {
      if (e.delta () > 0) {
        this->radiusEdit->stepUp ();
        return ToolResponse::Redraw;
      }
      else {
        this->radiusEdit->stepDown ();
        return ToolResponse::Redraw;
      }
    }
    return ToolResponse::None;
  }

  void runClose () {
    if (this->actions->isEmpty () == false) {
      this->self->state ().history ().addUnit (std::move (*this->actions));
    }
  }
};

DELEGATE_TOOL                         (ToolCarve)
DELEGATE_TOOL_RUN_RENDER              (ToolCarve)
DELEGATE_TOOL_RUN_MOUSE_MOVE_EVENT    (ToolCarve)
DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT (ToolCarve)
DELEGATE_TOOL_RUN_MOUSE_WHEEL_EVENT   (ToolCarve)
DELEGATE_TOOL_RUN_CLOSE               (ToolCarve)
