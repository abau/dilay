#include <QCheckBox>
#include <QDoubleSpinBox>
#include "action/sculpt.hpp"
#include "action/unit.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "config.hpp"
#include "history.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
#include "sculpt-brush.hpp"
#include "selection.hpp"
#include "state.hpp"
#include "tool/sculpt/behavior.hpp"
#include "view/cursor.hpp"
#include "view/properties.hpp"
#include "view/tool/tip.hpp"
#include "view/util.hpp"
#include "winged/face-intersection.hpp"
#include "winged/mesh.hpp"

struct ToolSculptBehavior::Impl {
  ToolSculptBehavior*          self;
  ConfigProxy                  config;
  State&                       state;
  std::unique_ptr <ActionUnit> actions;
  ViewCursor                   cursor;
  QDoubleSpinBox&              radiusEdit;

  Impl (ToolSculptBehavior* s, ConfigProxy& c, State& st, const char* key) 
    : self       (s)
    , config     (c, std::string (key) + "/")
    , state      (st)
    , actions    (new ActionUnit) 
    , cursor     (1.0f, Color::Red ())
    , radiusEdit (ViewUtil::spinBox (0.01f, 1.0f, 1000.0f, 1.0f))
  {}

  void setupBrushAndCursor () {
    this->setupBrush            (this->self->brush ());

    this->cursor.radius         (this->self->brush ().radius ());
    this->cursor.color          (this->config.get <Color> ("cursor-color", Color::Red ()));
    this->cursor.updateGeometry ();
   }


  void setupProperties (ViewProperties& properties) {
    SculptBrush& brush = this->self->brush ();

    this->radiusEdit.setValue (brush.radius ());
    ViewUtil::connect (this->radiusEdit, [this] (float r) {
      this->self->brush ().radius (r);
      this->cursor.radius (r);
      this->cursor.updateGeometry ();
      this->config.cache ("radius", r);
    });
    properties.addWidget (QObject::tr ("Radius"), this->radiusEdit);

    QDoubleSpinBox& detailEdit = ViewUtil::spinBox (0.01f, brush.detailFactor (), 0.95f, 0.1f);
    ViewUtil::connect (detailEdit, [this] (float h) {
      this->self->brush ().detailFactor (h);
      this->config.cache ("detail-factor", h);
    });
    properties.addWidget (QObject::tr ("Detail"), detailEdit);

    QDoubleSpinBox& stepEdit = ViewUtil::spinBox (0.01f, brush.stepWidthFactor (), 1000.0f, 0.1f);
    ViewUtil::connect (stepEdit, [this] (float s) {
      this->self->brush ().stepWidthFactor (s);
      this->config.cache ("step-width-factor", s);
    });
    properties.addWidget (QObject::tr ("Step width"), stepEdit);

    QCheckBox& subdivEdit = ViewUtil::checkBox (QObject::tr ("Subdivide"), brush.subdivide ());
    QObject::connect (&subdivEdit, &QCheckBox::stateChanged, [this] (int s) {
      this->self->brush ().subdivide (bool (s));
      this->config.cache ("subdivide", bool (s));
    });
    properties.addWidget (subdivEdit);

    this->self->runSetupProperties (properties);
  }

  void setupToolTip (ViewToolTip& toolTip) {
    this->self->runSetupToolTip (toolTip);
    toolTip.add ( ViewToolTip::MouseEvent::Wheel, ViewToolTip::Modifier::Shift
                , QObject::tr ("Change radius") );
  }

  void render () const {
    this->cursor.render (this->state.camera ());
    this->self->runRender ();
  }

  void mouseMoveEvent (const glm::ivec2& pos, bool leftButton) {
    this->self->runMouseMoveEvent (pos, leftButton);
  }

  void mouseLeftPressEvent (const glm::ivec2& pos) {
    this->self->runMouseLeftPressEvent (pos);
  }

  void mouseLeftReleaseEvent (const glm::ivec2& pos) {
    this->self->runMouseLeftReleaseEvent (pos);
    this->self->brush ().resetPosition ();
    this->addActionsToHistory ();
  }

  void addActionsToHistory () {
    if (this->actions->isEmpty () == false) {
      this->state.history ().addUnit (std::move (*this->actions));
      this->actions.reset (new ActionUnit ());
    }
  }

  void mouseWheelEvent (bool up) {
    if (up) {
      this->radiusEdit.stepUp ();
    }
    else {
      this->radiusEdit.stepDown ();
    }
    ViewUtil::deselect (this->radiusEdit);
  }

  void close () {
    this->addActionsToHistory ();
  }

  bool intersectsSelection (const PrimRay& ray, WingedFaceIntersection& intersection) const {
    return this->state.scene ().intersects (ray, intersection) 
        && this->state.scene ().selection  ().hasMajor (intersection.mesh ().index ());
  }

  bool intersectsSelection (const glm::ivec2& pos, WingedFaceIntersection& intersection) const {
    return this->intersectsSelection (this->state.camera ().ray (pos), intersection);
  }

  void setupBrush (SculptBrush& brush) const {
    brush.radius          (this->self->config ().get <float> ("radius"           , 10.0f));
    brush.detailFactor    (this->self->config ().get <float> ("detail-factor"    ,  0.6f));
    brush.stepWidthFactor (this->self->config ().get <float> ("step-width-factor",  0.3f));
    brush.subdivide       (this->self->config ().get <bool>  ("subdivide"        ,  true));
  }

  void sculpt (const SculptBrush& brush) {
    this->actions->add <ActionSculpt, WingedMesh> 
      (this->state.scene (), brush.meshRef ()).run (brush);
  }

  void sculpt () {
    this->sculpt (this->self->brush ());
  }
};

DELEGATE3_BIG3_SELF (ToolSculptBehavior, ConfigProxy&, State&, const char*)

GETTER_CONST    (ConfigProxy&   , ToolSculptBehavior, config)
GETTER_CONST    (State&         , ToolSculptBehavior, state)
GETTER_CONST    (ViewCursor&    , ToolSculptBehavior, cursor)
DELEGATE        (void           , ToolSculptBehavior, setupBrushAndCursor)
DELEGATE1       (void           , ToolSculptBehavior, setupProperties, ViewProperties&)
DELEGATE1       (void           , ToolSculptBehavior, setupToolTip, ViewToolTip&)
DELEGATE_CONST  (void           , ToolSculptBehavior, render)
DELEGATE2       (void           , ToolSculptBehavior, mouseMoveEvent, const glm::ivec2&, bool)
DELEGATE1       (void           , ToolSculptBehavior, mouseLeftPressEvent, const glm::ivec2&)
DELEGATE1       (void           , ToolSculptBehavior, mouseLeftReleaseEvent, const glm::ivec2&)
DELEGATE1       (void           , ToolSculptBehavior, mouseWheelEvent, bool)
DELEGATE        (void           , ToolSculptBehavior, close)
DELEGATE2_CONST (bool           , ToolSculptBehavior, intersectsSelection, const PrimRay&, WingedFaceIntersection&)
DELEGATE2_CONST (bool           , ToolSculptBehavior, intersectsSelection, const glm::ivec2&, WingedFaceIntersection&)
DELEGATE1_CONST (void           , ToolSculptBehavior, setupBrush, SculptBrush&)
DELEGATE1       (void           , ToolSculptBehavior, sculpt, const SculptBrush&)
DELEGATE        (void           , ToolSculptBehavior, sculpt)
