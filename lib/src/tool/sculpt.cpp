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
#include "state.hpp"
#include "tool/sculpt.hpp"
#include "view/cursor.hpp"
#include "view/properties.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"
#include "winged/face-intersection.hpp"
#include "winged/mesh.hpp"

struct ToolSculpt::Impl {
  ToolSculpt*                  self;
  CacheProxy                   commonCache;
  std::unique_ptr <ActionUnit> actions;
  QDoubleSpinBox&              radiusEdit;

  Impl (ToolSculpt* s) 
    : self        (s) 
    , commonCache (this->self->cache ("sculpt"))
    , actions     (new ActionUnit) 
    , radiusEdit  (ViewUtil::spinBox (0.01f, 1.0f, 1000.0f, 10.0f))
  {}

  bool runAllowUndoRedo () const {
    return true;
  }

  ToolResponse runInitialize () {
    this->setupBrush      ();
    this->setupCursor     ();
    this->setupProperties ();
    this->setupToolTip    ();

    return ToolResponse::Redraw;
  }

  void setupBrush () {
    SculptBrush& brush = this->self->brush ();

    brush.radius          (this->commonCache.get <float> ("radius"           , 20.0f));
    brush.detailFactor    (this->commonCache.get <float> ("detail-factor"    , 0.7f));
    brush.stepWidthFactor (this->commonCache.get <float> ("step-width-factor", 0.3f));
    brush.subdivide       (this->commonCache.get <bool>  ("subdivide"        , true));

    this->self->runSetupBrush ();
  }

  void setupCursor () {
    assert (this->self->brush ().radius () > 0.0f);

    WingedFaceIntersection intersection;
    if (this->self->intersectsScene (this->self->cursorPosition (), intersection)) {
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

  void setupProperties () {
    ViewPropertiesPart& properties = this->self->properties ().body ();
    SculptBrush&        brush      = this->self->brush ();

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

    QCheckBox& subdivEdit = ViewUtil::checkBox (QObject::tr ("Subdivide"), brush.subdivide ());
    QObject::connect (&subdivEdit, &QCheckBox::stateChanged, [this] (int s) {
      this->self->brush ().subdivide (bool (s));
      this->commonCache.set ("subdivide", bool (s));
    });
    properties.add (subdivEdit);

    properties.add (ViewUtil::horizontalLine ());

    this->self->runSetupProperties (properties);
  }

  void setupToolTip () {
    ViewToolTip toolTip;

    this->self->runSetupToolTip (toolTip);
    toolTip.add ( ViewToolTip::MouseEvent::Wheel, ViewToolTip::Modifier::Shift
                , QObject::tr ("Change radius") );

    this->self->showToolTip (toolTip);
  }

  void runRender () const {
    this->self->cursor ().render (this->self->state ().camera ());
  }

  ToolResponse runMouseReleaseEvent (const QMouseEvent& e) {
    if (e.button () == Qt::LeftButton) {
      this->self->brush ().resetPosition ();
      this->addActionsToHistory ();
    }
    return ToolResponse::None;
  }

  void addActionsToHistory () {
    assert (this->actions);
    if (this->actions->isEmpty () == false) {
      this->self->state ().history ().addUnit (std::move (*this->actions));
      this->actions.reset (new ActionUnit ());
    }
  }

  ToolResponse runWheelEvent (const QWheelEvent& e) {
    if (e.orientation () == Qt::Vertical && e.modifiers () == Qt::ShiftModifier) {
      if (e.delta () > 0) {
        this->radiusEdit.stepUp ();
      }
      else if (e.delta () < 0) {
        this->radiusEdit.stepDown ();
      }
      ViewUtil::deselect (this->radiusEdit);
    }
    return ToolResponse::Redraw;
  }

  void runClose () {
    this->addActionsToHistory ();
  }

  void sculpt () {
    SculptBrush& brush = this->self->brush ();

    this->actions->add <ActionSculpt> ( this->self->state ().scene ()
                                      , brush.meshRef () ).run (brush);
  }

  void updateCursorByIntersection (const QMouseEvent& e) {
    WingedFaceIntersection intersection;

    if (this->self->intersectsScene (e, intersection)) {
      this->self->cursor ().enable   ();
      this->self->cursor ().position (intersection.position ());
      this->self->cursor ().normal   (intersection.normal   ());
    }
    else {
      this->self->cursor ().disable ();
    }
  }

  bool updateBrushAndCursorByIntersection (const QMouseEvent& e) {
    WingedFaceIntersection intersection;

    if (this->self->intersectsScene (e, intersection)) {
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

DELEGATE_BIG2_BASE (ToolSculpt, (State& s, const char* k), (this), Tool, (s, k))
DELEGATE       (void        , ToolSculpt, sculpt)
DELEGATE1      (void        , ToolSculpt, updateCursorByIntersection, const QMouseEvent&)
DELEGATE1      (bool        , ToolSculpt, updateBrushAndCursorByIntersection, const QMouseEvent&)
DELEGATE_CONST (bool        , ToolSculpt, runAllowUndoRedo)
DELEGATE       (ToolResponse, ToolSculpt, runInitialize)
DELEGATE_CONST (void        , ToolSculpt, runRender)
DELEGATE1      (ToolResponse, ToolSculpt, runMouseReleaseEvent, const QMouseEvent&)
DELEGATE1      (ToolResponse, ToolSculpt, runWheelEvent, const QWheelEvent&)
DELEGATE       (void        , ToolSculpt, runClose)
