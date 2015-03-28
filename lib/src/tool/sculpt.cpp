#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QMouseEvent>
#include <QWheelEvent>
#include "action/sculpt.hpp"
#include "cache.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "config.hpp"
#include "history.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
#include "sculpt-brush.hpp"
#include "state.hpp"
#include "tool/sculpt.hpp"
#include "tool/util/movement.hpp"
#include "view/cursor.hpp"
#include "view/properties.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"
#include "winged/face-intersection.hpp"
#include "winged/mesh.hpp"

struct ToolSculpt::Impl {
  ToolSculpt*     self;
  SculptBrush     brush;
  ViewCursor      cursor;
  CacheProxy      commonCache;
  QDoubleSpinBox& radiusEdit;

  Impl (ToolSculpt* s) 
    : self        (s) 
    , commonCache (this->self->cache ("sculpt"))
    , radiusEdit  (ViewUtil::spinBox (0.01f, 1.0f, 1000.0f, 5.0f))
  {}

  ToolResponse runInitialize () {
    this->setupBrush      ();
    this->setupCursor     ();
    this->setupProperties ();
    this->setupToolTip    ();

    return ToolResponse::Redraw;
  }

  void setupBrush () {
    const Config&     config = this->self->config ();
    const CacheProxy& cCache = this->commonCache;

    this->brush.detailFactor    (config.get <float> ("editor/tool/sculpt/detail-factor"));
    this->brush.stepWidthFactor (config.get <float> ("editor/tool/sculpt/step-width-factor"));

    this->brush.radius          (cCache.get <float> ("radius"   , 20.0f));
    this->brush.subdivide       (cCache.get <bool>  ("subdivide", true));

    this->self->runSetupBrush (this->brush);
  }

  void setupCursor () {
    assert (this->brush.radius () > 0.0f);

    WingedFaceIntersection intersection;
    if (this->self->intersectsScene (this->self->cursorPosition (), intersection)) {
      this->cursor.enable   ();
      this->cursor.position (intersection.position ());
    }
    else {
      this->cursor.disable ();
    }
    this->cursor.radius (this->brush.radius ());
    this->cursor.color  (this->self->config ().get <Color> ("editor/tool/sculpt/cursor-color"));

    this->self->runSetupCursor (this->cursor);
  }

  void setupProperties () {
    ViewPropertiesPart& properties = this->self->properties ().body ();

    this->radiusEdit.setValue (this->brush.radius ());
    ViewUtil::connect (this->radiusEdit, [this] (float r) {
      this->brush.radius (r);
      this->cursor.radius (r);
      this->commonCache.set ("radius", r);
    });
    properties.add (QObject::tr ("Radius"), this->radiusEdit);

    QCheckBox& subdivEdit = ViewUtil::checkBox ( QObject::tr ("Subdivide")
                                               , this->brush.subdivide () );
    ViewUtil::connect (subdivEdit, [this] (bool s) {
      this->brush.subdivide (s);
      this->commonCache.set ("subdivide", s);
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
    this->cursor.render (this->self->state ().camera ());
  }

  ToolResponse runMouseMoveEvent (const QMouseEvent& e) {
    this->self->runSculptMouseMoveEvent (e);
    return ToolResponse::Redraw;
  }

  ToolResponse runMousePressEvent (const QMouseEvent& e) {
    this->self->snapshotScene ();

    if (this->self->runSculptMousePressEvent (e) == false) {
      this->self->state ().history ().dropSnapshot ();
    }
    return ToolResponse::Redraw;
  }

  ToolResponse runMouseReleaseEvent (const QMouseEvent& e) {
    if (e.button () == Qt::LeftButton) {
      this->brush.resetPointOfAction ();
    }
    this->cursor.enable ();
    return ToolResponse::None;
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

  void runClose () {}

  void sculpt () {
    Action::sculpt (this->brush);
  }

  void updateCursorByIntersection (const QMouseEvent& e) {
    WingedFaceIntersection intersection;

    if (this->self->intersectsScene (e, intersection)) {
      this->cursor.enable   ();
      this->cursor.position (intersection.position ());
    }
    else {
      this->cursor.disable ();
    }
  }

  bool updateBrushAndCursorByIntersection (const QMouseEvent& e) {
    WingedFaceIntersection intersection;

    if (this->self->intersectsScene (e, intersection)) {
      this->cursor.enable   ();
      this->cursor.position (intersection.position ());

      if (e.button () == Qt::LeftButton || e.buttons () == Qt::LeftButton) {
        this->brush.mesh (&intersection.mesh ());

        return this->brush.updatePointOfAction (intersection.position (), intersection.normal ());
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

  bool carvelikeStroke (const QMouseEvent& e, const std::function <void ()>* toggle) {
    if (this->updateBrushAndCursorByIntersection (e)) {
      if (toggle && e.modifiers () == Qt::ShiftModifier) {
        (*toggle) ();
        this->sculpt ();
        (*toggle) ();
      }
      else {
        this->sculpt ();
      }
      return true;
    }
    else {
      return false;
    }
  }

  bool initializeDraglikeStroke (const QMouseEvent& e, ToolUtilMovement& movement) {
    if (e.button () == Qt::LeftButton) {
      WingedFaceIntersection intersection;
      if (this->self->intersectsScene (e, intersection)) {
        this->brush.mesh (&intersection.mesh ());
        this->brush.setPointOfAction (intersection.position (), intersection.normal ());
        
        this->cursor.disable ();

        movement.resetPosition (intersection.position ());
        movement.constraint    (MovementConstraint::CameraPlane);
        return true;
      }
      else {
        this->cursor.enable ();
        this->brush.resetPointOfAction ();
        return false;
      }
    }
    else {
      this->cursor.enable ();
      this->brush.resetPointOfAction ();
      return false;
    }
  }

  bool draglikeStroke (const QMouseEvent& e, ToolUtilMovement& movement) {
    if (e.buttons () == Qt::NoButton) {
      this->updateCursorByIntersection (e);
      return false;
    }
    else if (e.buttons () == Qt::LeftButton && this->brush.hasPosition ()) {
      const glm::vec3 oldBrushPos = this->brush.position ();

      if ( movement.move (ViewUtil::toIVec2 (e))
        && this->brush.updatePointOfAction ( movement.position ()
                                           , movement.position () - oldBrushPos ) )
      {
        auto& params = this->brush.parameters <SBMoveDirectionalParameters> ();

        params.useAverageNormal (false);
        params.intensityFactor  (1.0f / this->brush.radius ());
        this->sculpt ();
        return true;
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

DELEGATE_BIG2_BASE (ToolSculpt, (State& s, const char* k), (this), Tool, (s, k))
GETTER         (SculptBrush&, ToolSculpt, brush)
GETTER         (ViewCursor& , ToolSculpt, cursor)
DELEGATE       (void        , ToolSculpt, sculpt)
DELEGATE1      (void        , ToolSculpt, updateCursorByIntersection, const QMouseEvent&)
DELEGATE1      (bool        , ToolSculpt, updateBrushAndCursorByIntersection, const QMouseEvent&)
DELEGATE2      (bool        , ToolSculpt, carvelikeStroke, const QMouseEvent&, const std::function <void ()>*)
DELEGATE2      (bool        , ToolSculpt, initializeDraglikeStroke, const QMouseEvent&, ToolUtilMovement&)
DELEGATE2      (bool        , ToolSculpt, draglikeStroke, const QMouseEvent&, ToolUtilMovement&)
DELEGATE       (ToolResponse, ToolSculpt, runInitialize)
DELEGATE_CONST (void        , ToolSculpt, runRender)
DELEGATE1      (ToolResponse, ToolSculpt, runMouseMoveEvent, const QMouseEvent&)
DELEGATE1      (ToolResponse, ToolSculpt, runMousePressEvent, const QMouseEvent&)
DELEGATE1      (ToolResponse, ToolSculpt, runMouseReleaseEvent, const QMouseEvent&)
DELEGATE1      (ToolResponse, ToolSculpt, runWheelEvent, const QWheelEvent&)
DELEGATE       (void        , ToolSculpt, runClose)
