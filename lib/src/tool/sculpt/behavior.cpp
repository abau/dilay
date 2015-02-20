#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QMouseEvent>
#include <QWheelEvent>
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
  ConfigProxy                  commonConfig;
  ConfigProxy                  config;
  State&                       state;
  std::unique_ptr <ActionUnit> actions;
  ViewCursor                   cursor;
  QDoubleSpinBox&              radiusEdit;

  Impl (ToolSculptBehavior* s, ConfigProxy& c, State& st, const char* key) 
    : self         (s)
    , commonConfig (c)
    , config       (c, std::string (key) + "/")
    , state        (st)
    , actions      (new ActionUnit) 
    , cursor       (1.0f, Color::Red ())
    , radiusEdit   (ViewUtil::spinBox (0.01f, 1.0f, 1000.0f, 1.0f))
  {}

  void setupCursor (const glm::ivec2& pos) {
    assert (this->self->brush ().radius () > 0.0f);

    WingedFaceIntersection intersection;

    if (this->self->intersectsSelection (pos, intersection)) {
      this->cursor.position (intersection.position ());
      this->cursor.normal   (intersection.normal   ());
    }
    this->cursor.radius         (this->self->brush ().radius ());
    this->cursor.color          (this->commonConfig.get <Color> ("cursor-color", Color::Red ()));
    this->cursor.updateGeometry ();
   }


  void setupProperties (ViewPropertiesPart& properties) {
    SculptBrush& brush = this->self->brush ();

    this->radiusEdit.setValue (brush.radius ());
    ViewUtil::connect (this->radiusEdit, [this] (float r) {
      this->self->brush ().radius (r);
      this->cursor.radius (r);
      this->cursor.updateGeometry ();
      this->commonConfig.cache ("radius", r);
    });
    properties.add (QObject::tr ("Radius"), this->radiusEdit);

    QDoubleSpinBox& detailEdit = ViewUtil::spinBox (0.01f, brush.detailFactor (), 0.95f, 0.1f);
    ViewUtil::connect (detailEdit, [this] (float h) {
      this->self->brush ().detailFactor (h);
      this->commonConfig.cache ("detail-factor", h);
    });
    properties.add (QObject::tr ("Detail"), detailEdit);

    QDoubleSpinBox& stepEdit = ViewUtil::spinBox (0.01f, brush.stepWidthFactor (), 1000.0f, 0.1f);
    ViewUtil::connect (stepEdit, [this] (float s) {
      this->self->brush ().stepWidthFactor (s);
      this->commonConfig.cache ("step-width-factor", s);
    });
    properties.add (QObject::tr ("Step width"), stepEdit);

    QCheckBox& subdivEdit = ViewUtil::checkBox (QObject::tr ("Subdivide"), brush.subdivide ());
    QObject::connect (&subdivEdit, &QCheckBox::stateChanged, [this] (int s) {
      this->self->brush ().subdivide (bool (s));
      this->commonConfig.cache ("subdivide", bool (s));
    });
    properties.add (subdivEdit);

    properties.add (ViewUtil::horizontalLine ());

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

  void mouseMoveEvent (const QMouseEvent& e) {
    this->self->runMouseMoveEvent (e);
  }

  void mousePressEvent (const QMouseEvent& e) {
    this->self->runMousePressEvent (e);
  }

  void mouseReleaseEvent (const QMouseEvent& e) {
    this->self->runMouseReleaseEvent (e);
    if (e.button () == Qt::LeftButton) {
      this->self->brush ().resetPosition ();
      this->addActionsToHistory ();
    }
  }

  void addActionsToHistory () {
    if (this->actions->isEmpty () == false) {
      this->state.history ().addUnit (std::move (*this->actions));
      this->actions.reset (new ActionUnit ());
    }
  }

  void mouseWheelEvent (const QWheelEvent& e) {
    if (e.orientation () == Qt::Vertical && e.modifiers ().testFlag (Qt::ShiftModifier)) {
      if (e.delta () > 0) {
        this->radiusEdit.stepUp ();
      }
      else if (e.delta () < 0) {
        this->radiusEdit.stepDown ();
      }
      ViewUtil::deselect (this->radiusEdit);
    }
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

  void brushFromCommonCache () {
    SculptBrush& brush = this->self->brush ();

    brush.radius          (this->commonConfig.get <float> ("radius"           , 20.0f));
    brush.detailFactor    (this->commonConfig.get <float> ("detail-factor"    , 0.7f));
    brush.stepWidthFactor (this->commonConfig.get <float> ("step-width-factor", 0.3f));
    brush.subdivide       (this->commonConfig.get <bool>  ("subdivide"        , true));
  }

  void sculpt () {
    SculptBrush& brush = this->self->brush ();

    this->actions->add <ActionSculpt, WingedMesh> 
      (this->state.scene (), brush.meshRef ()).run (brush);
  }

  bool updateBrushByIntersection (const QMouseEvent& e) {
    WingedFaceIntersection intersection;

    if (this->intersectsSelection (ViewUtil::toIVec2 (e), intersection)) {
      this->cursor.enable   ();
      this->cursor.position (intersection.position ());
      this->cursor.normal   (intersection.normal   ());

      if (e.button () == Qt::LeftButton || e.buttons () == Qt::LeftButton) {
        this->self->brush ().mesh (&intersection.mesh ());
        this->self->brush ().face (&intersection.face ());

        return this->self->brush ().updatePosition (intersection.position ());
      }
      else {
        return false;
      }
    }
    else {
      this->cursor.disable ();
      return false;
    }
  }
};

DELEGATE3_BIG3_SELF (ToolSculptBehavior, ConfigProxy&, State&, const char*)

GETTER_CONST    (ConfigProxy&   , ToolSculptBehavior, config)
GETTER_CONST    (State&         , ToolSculptBehavior, state)
GETTER_CONST    (ViewCursor&    , ToolSculptBehavior, cursor)
DELEGATE1       (void           , ToolSculptBehavior, setupCursor, const glm::ivec2&)
DELEGATE1       (void           , ToolSculptBehavior, setupProperties, ViewPropertiesPart&)
DELEGATE1       (void           , ToolSculptBehavior, setupToolTip, ViewToolTip&)
DELEGATE_CONST  (void           , ToolSculptBehavior, render)
DELEGATE1       (void           , ToolSculptBehavior, mouseMoveEvent, const QMouseEvent&)
DELEGATE1       (void           , ToolSculptBehavior, mousePressEvent, const QMouseEvent&)
DELEGATE1       (void           , ToolSculptBehavior, mouseReleaseEvent, const QMouseEvent&)
DELEGATE1       (void           , ToolSculptBehavior, mouseWheelEvent, const QWheelEvent&)
DELEGATE        (void           , ToolSculptBehavior, close)
DELEGATE2_CONST (bool           , ToolSculptBehavior, intersectsSelection, const PrimRay&, WingedFaceIntersection&)
DELEGATE2_CONST (bool           , ToolSculptBehavior, intersectsSelection, const glm::ivec2&, WingedFaceIntersection&)
DELEGATE        (void           , ToolSculptBehavior, brushFromCommonCache)
DELEGATE        (void           , ToolSculptBehavior, sculpt)
DELEGATE1       (bool           , ToolSculptBehavior, updateBrushByIntersection, const QMouseEvent&)
