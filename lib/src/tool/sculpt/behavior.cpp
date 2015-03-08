#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QMouseEvent>
#include <QWheelEvent>
#include "action/sculpt.hpp"
#include "action/unit.hpp"
#include "cache.hpp"
#include "camera.hpp"
#include "color.hpp"
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
  CacheProxy                   commonCache;
  CacheProxy                   cache;
  State&                       state;
  std::unique_ptr <ActionUnit> actions;
  QDoubleSpinBox&              radiusEdit;
  QCheckBox&                   subdivEdit;

  Impl (ToolSculptBehavior* s, CacheProxy& c, State& st, const char* key) 
    : self        (s)
    , commonCache (c)
    , cache       (c, std::string (key) + "/")
    , state       (st)
    , actions     (new ActionUnit) 
    , radiusEdit  (ViewUtil::spinBox (0.01f, 1.0f, 1000.0f, 10.0f))
    , subdivEdit  (ViewUtil::checkBox (QObject::tr ("Subdivide"), true))
  {}

  void setupBrush () {
    SculptBrush& brush = this->self->brush ();

    brush.radius          (this->commonCache.get <float> ("radius"           , 20.0f));
    brush.detailFactor    (this->commonCache.get <float> ("detail-factor"    , 0.7f));
    brush.stepWidthFactor (this->commonCache.get <float> ("step-width-factor", 0.3f));
    brush.subdivide       (this->commonCache.get <bool>  ("subdivide"        , true));

    this->self->runSetupBrush ();
  }

  void setupCursor (const glm::ivec2& pos) {
    assert (this->self->brush ().radius () > 0.0f);

    WingedFaceIntersection intersection;

    if (this->self->intersectsSelection (pos, intersection)) {
      this->self->cursor ().position (intersection.position ());
      this->self->cursor ().normal   (intersection.normal   ());
    }
    else {
      this->self->cursor ().disable ();
    }
    this->self->cursor ().radius (this->self->brush ().radius ());
    this->self->cursor ().color  (this->commonCache.get <Color> ("cursor-color", Color::Red ()));

    this->self->runSetupCursor ();
  }

  void setupProperties (ViewPropertiesPart& properties) {
    SculptBrush& brush = this->self->brush ();

    this->radiusEdit.setValue (brush.radius ());
    ViewUtil::connect (this->radiusEdit, [this] (float r) {
      this->self->brush  ().radius (r);
      this->self->cursor ().radius (r);
      this->commonCache.set ("radius", r);
    });
    properties.add (QObject::tr ("Radius"), this->radiusEdit);

    QDoubleSpinBox& detailEdit = ViewUtil::spinBox (0.01f, brush.detailFactor (), 0.95f, 0.1f);
    ViewUtil::connect (detailEdit, [this] (float h) {
      this->self->brush ().detailFactor (h);
      this->commonCache.set ("detail-factor", h);
    });
    properties.add (QObject::tr ("Detail"), detailEdit);

    QDoubleSpinBox& stepEdit = ViewUtil::spinBox (0.01f, brush.stepWidthFactor (), 1000.0f, 0.1f);
    ViewUtil::connect (stepEdit, [this] (float s) {
      this->self->brush ().stepWidthFactor (s);
      this->commonCache.set ("step-width-factor", s);
    });
    properties.add (QObject::tr ("Step width"), stepEdit);

    this->subdivEdit.setChecked (brush.subdivide ());
    QObject::connect (&this->subdivEdit, &QCheckBox::stateChanged, [this] (int s) {
      this->self->brush ().subdivide (bool (s));
      this->commonCache.set ("subdivide", bool (s));
    });
    properties.add (this->subdivEdit);

    properties.add (ViewUtil::horizontalLine ());

    this->self->runSetupProperties (properties);
  }

  void setupToolTip (ViewToolTip& toolTip) {
    this->self->runSetupToolTip (toolTip);
    toolTip.add ( ViewToolTip::MouseEvent::Wheel, ViewToolTip::Modifier::Shift
                , QObject::tr ("Change radius") );
  }

  void render () const {
    this->self->cursor ().render (this->state.camera ());
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

  bool intersectsSelection (const QMouseEvent& e, WingedFaceIntersection& intersection) const {
    return this->intersectsSelection (ViewUtil::toIVec2 (e), intersection );
  }

  void forceBrushSubdivision (bool value) {
    this->self->brush ().subdivide (value);
    this->subdivEdit.setChecked    (value);
    this->subdivEdit.setEnabled    (false);
  }

  void sculpt () {
    SculptBrush& brush = this->self->brush ();

    this->actions->add <ActionSculpt, WingedMesh> 
      (this->state.scene (), brush.meshRef ()).run (brush);
  }

  void updateCursorByIntersection (const QMouseEvent& e) {
    WingedFaceIntersection intersection;

    if (this->intersectsSelection (e, intersection)) {
      this->self->cursor ().enable   ();
      this->self->cursor ().position (intersection.position ());
      this->self->cursor ().normal   (intersection.normal   ());
    }
    else {
      this->self->cursor ().disable ();
    }
  }

  bool updateBrushByIntersection (const QMouseEvent& e) {
    WingedFaceIntersection intersection;

    if (this->intersectsSelection (e, intersection)) {
      this->self->cursor ().enable   ();
      this->self->cursor ().position (intersection.position ());
      this->self->cursor ().normal   (intersection.normal   ());

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
      this->self->cursor ().disable ();
      return false;
    }
  }
};

DELEGATE3_BIG3_SELF (ToolSculptBehavior, CacheProxy&, State&, const char*)

GETTER_CONST    (CacheProxy&, ToolSculptBehavior, cache)
GETTER_CONST    (State&     , ToolSculptBehavior, state)
DELEGATE        (void       , ToolSculptBehavior, setupBrush)
DELEGATE1       (void       , ToolSculptBehavior, setupCursor, const glm::ivec2&)
DELEGATE1       (void       , ToolSculptBehavior, setupProperties, ViewPropertiesPart&)
DELEGATE1       (void       , ToolSculptBehavior, setupToolTip, ViewToolTip&)
DELEGATE_CONST  (void       , ToolSculptBehavior, render)
DELEGATE1       (void       , ToolSculptBehavior, mouseMoveEvent, const QMouseEvent&)
DELEGATE1       (void       , ToolSculptBehavior, mousePressEvent, const QMouseEvent&)
DELEGATE1       (void       , ToolSculptBehavior, mouseReleaseEvent, const QMouseEvent&)
DELEGATE1       (void       , ToolSculptBehavior, mouseWheelEvent, const QWheelEvent&)
DELEGATE        (void       , ToolSculptBehavior, close)
DELEGATE2_CONST (bool       , ToolSculptBehavior, intersectsSelection, const PrimRay&, WingedFaceIntersection&)
DELEGATE2_CONST (bool       , ToolSculptBehavior, intersectsSelection, const glm::ivec2&, WingedFaceIntersection&)
DELEGATE2_CONST (bool       , ToolSculptBehavior, intersectsSelection, const QMouseEvent&, WingedFaceIntersection&)
DELEGATE1       (void       , ToolSculptBehavior, forceBrushSubdivision, bool)
DELEGATE        (void       , ToolSculptBehavior, sculpt)
DELEGATE1       (void       , ToolSculptBehavior, updateCursorByIntersection, const QMouseEvent&)
DELEGATE1       (bool       , ToolSculptBehavior, updateBrushByIntersection, const QMouseEvent&)
