/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCheckBox>
#include <QFrame>
#include <QPushButton>
#include <QWheelEvent>
#include "action/sculpt.hpp"
#include "cache.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "history.hpp"
#include "mirror.hpp"
#include "scene.hpp"
#include "sculpt-brush.hpp"
#include "state.hpp"
#include "tool/sculpt.hpp"
#include "tool/util/movement.hpp"
#include "view/cursor.hpp"
#include "view/double-slider.hpp"
#include "view/pointing-event.hpp"
#include "view/tool-tip.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"
#include "winged/face-intersection.hpp"

struct ToolSculpt::Impl {
  ToolSculpt*       self;
  SculptBrush       brush;
  ViewCursor        cursor;
  CacheProxy        commonCache;
  ViewDoubleSlider& radiusEdit;
  ViewDoubleSlider* secondarySlider;
  bool              absoluteRadius;
  bool              sculpted;

  Impl (ToolSculpt* s) 
    : self            (s)
    , commonCache     (this->self->cache ("sculpt"))
    , radiusEdit      (ViewUtil::slider  (2, 0.01f
                                           , this->commonCache.get <float> ("radius", 0.1f)
                                           , 1.0f, 3))
    , secondarySlider (nullptr)
    , absoluteRadius  (this->commonCache.get <bool> ("absolute-radius", true))
    , sculpted        (false)
  {}

  ToolResponse runInitialize () {
    this->setupBrush      ();
    this->setupCursor     ();
    this->setupProperties ();
    this->setupToolTip    ();

    return ToolResponse::Redraw;
  }

  void setupBrush () {
    this->brush.subdivide (this->commonCache.get <bool> ("subdivide", true));

    if (this->absoluteRadius) {
      this->setAbsoluteRadius ();
    }
    else {
      this->setRelativeRadius ();
    }
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

    this->self->runSetupCursor (this->cursor);
  }

  void setupProperties () {
    ViewTwoColumnGrid& properties = this->self->makeProperties ();

    ViewUtil::connect (this->radiusEdit, [this] (float r) {
      if (this->absoluteRadius) {
        this->setAbsoluteRadius ();
      }
      else {
        this->setRelativeRadius ();
      }
      this->commonCache.set ("radius", r);
      this->self->updateGlWidget ();
    });
    properties.addStacked (QObject::tr ("Radius"), this->radiusEdit);

    QCheckBox& absRadiusEdit = ViewUtil::checkBox ( QObject::tr ("Absolute radius")
                                                  , this->absoluteRadius );
    ViewUtil::connect (absRadiusEdit, [this] (bool a) {
      if (a) {
        this->setAbsoluteRadius ();
      }
      else {
        this->setRelativeRadius ();
      }
      this->commonCache.set ("absolute-radius", a);
      this->self->updateGlWidget ();
    });
    properties.add (absRadiusEdit);

    QCheckBox& subdivEdit = ViewUtil::checkBox ( QObject::tr ("Subdivide")
                                               , this->brush.subdivide () );
    ViewUtil::connect (subdivEdit, [this] (bool s) {
      this->brush.subdivide (s);
      this->commonCache.set ("subdivide", s);
    });
    properties.add (subdivEdit);

    QPushButton& syncButton = ViewUtil::pushButton (QObject::tr ("Sync"));
    ViewUtil::connect (syncButton, [this] () {
      this->self->mirrorWingedMeshes ();
      this->self->updateGlWidget ();
    });
    syncButton.setEnabled (this->self->hasMirror ());

    QCheckBox& mirrorEdit = ViewUtil::checkBox ( QObject::tr ("Mirror")
                                               , this->self->hasMirror () );
    ViewUtil::connect (mirrorEdit, [this,&syncButton] (bool m) {
      this->self->mirror (m);
      syncButton.setEnabled (m);
    });

    properties.add (mirrorEdit, syncButton);

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
    Camera& camera = this->self->state ().camera ();

    if (this->cursor.isEnabled ()) {
      this->cursor.render (camera);
    }
  }

  ToolResponse runPointingEvent (const ViewPointingEvent& e) {
    if (e.releaseEvent ()) {
      if (e.primaryButton ()) {
        this->brush.resetPointOfAction ();

        if (this->sculpted == false) {
          this->self->state ().history ().dropSnapshot ();
        }
      }
      this->cursor.enable ();
      return ToolResponse::Redraw;
    }
    else {
      if (e.pressEvent () && e.primaryButton ()) {
        this->self->snapshotWingedMeshes ();
        this->sculpted = false;
      }

      if (this->self->runSculptPointingEvent (e)) {
        this->sculpted = true;
      }
      return ToolResponse::Redraw;
    }
  }

  ToolResponse runWheelEvent (const QWheelEvent& e) {
    auto updateSlider = [&e] (ViewDoubleSlider& slider) {
      if (e.delta () > 0) {
        slider.setIntValue (slider.intValue () + slider.intSingleStep ());
      }
      else if (e.delta () < 0) {
        slider.setIntValue (slider.intValue () - slider.intSingleStep ());
      }
    };

    if (e.orientation () == Qt::Vertical) {
      if (e.modifiers () == Qt::ShiftModifier) {
        updateSlider (this->radiusEdit);
      }
      else if (this->secondarySlider && e.modifiers () == Qt::ControlModifier) {
        updateSlider (*this->secondarySlider);
      }
    }
    return ToolResponse::Redraw;
  }

  ToolResponse runCursorUpdate (const glm::ivec2& pos) {
    this->updateBrushAndCursorByIntersection (pos, false, false);
    return ToolResponse::Redraw;
  }

  void runFromConfig () {
    const Config& config = this->self->config ();

    this->brush.detailFactor    (config.get <float> ("editor/tool/sculpt/detail-factor"));
    this->brush.stepWidthFactor (config.get <float> ("editor/tool/sculpt/step-width-factor"));

    this->cursor.color  (this->self->config ().get <Color> ("editor/tool/sculpt/cursor-color"));
  }

  void addDefaultToolTip (ViewToolTip& toolTip, bool hasInvertedMode) {
    toolTip.add (ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to sculpt"));

    if (hasInvertedMode) {
      toolTip.add ( ViewToolTip::MouseEvent::Left
                  , ViewToolTip::Modifier::Shift, QObject::tr ("Drag to sculpt inverted"));
    }
  }

  void addSecSliderWheelToolTip (ViewToolTip& toolTip, const QString& label) {
    toolTip.add (ViewToolTip::MouseEvent::Wheel, ViewToolTip::Modifier::Ctrl, label);
  }

  void sculpt () {
    Action::sculpt (this->brush);
    if (this->self->hasMirror ()) {
      this->brush.mirror (this->self->mirror ().plane ());
      Action::sculpt (this->brush);
      this->brush.mirror (this->self->mirror ().plane ());
    }
  }

  bool updateBrushAndCursorByIntersection ( const glm::ivec2& pos, bool buttonPressed
                                          , bool useRecentOctree )
  {
    WingedFaceIntersection intersection;

    if (this->self->intersectsScene (pos, intersection)) {
      this->cursor.enable   ();
      this->cursor.position (intersection.position ());

      if (this->absoluteRadius == false) {
        this->setRelativeRadius (intersection.distance ());
      }

      if (buttonPressed) {
        this->brush.mesh (&intersection.mesh ());

        if (useRecentOctree) {
          Intersection octreeIntersection;
          if (this->self->intersectsRecentOctree (pos, octreeIntersection)) {
            return this->brush.updatePointOfAction ( octreeIntersection.position ()
                                                   , octreeIntersection.normal () );
          }
          else {
            return this->brush.updatePointOfAction ( intersection.position ()
                                                   , intersection.normal () );
          }
        }
        else {
          return this->brush.updatePointOfAction ( intersection.position ()
                                                 , intersection.normal () );
        }
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

  bool updateBrushAndCursorByIntersection (const ViewPointingEvent& e, bool useRecentOctree) {
    return this->updateBrushAndCursorByIntersection ( e.ivec2 (), e.primaryButton ()
                                                    , useRecentOctree );
  }

  bool carvelikeStroke ( const ViewPointingEvent& e, bool useRecentOctree
                       , const std::function <void ()>* toggle )
  {
    if (this->updateBrushAndCursorByIntersection (e, useRecentOctree)) {
      const float defaultIntesity = this->brush.intensity ();

      this->brush.intensity (defaultIntesity * e.intensity ());

      if (toggle && e.modifiers () == Qt::ShiftModifier) {
        (*toggle) ();
        this->sculpt ();
        (*toggle) ();
      }
      else {
        this->sculpt ();
      }

      this->brush.intensity (defaultIntesity);
      return true;
    }
    else {
      return false;
    }
  }

  bool initializeDraglikeStroke (const ViewPointingEvent& e, ToolUtilMovement& movement) {
    if (e.primaryButton ()) {
      WingedFaceIntersection intersection;
      if (this->self->intersectsScene (e, intersection)) {
        this->brush.mesh (&intersection.mesh ());
        this->brush.setPointOfAction (intersection.position (), intersection.normal ());
        
        this->cursor.disable ();
        movement.resetPosition (intersection.position ());
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

  bool draglikeStroke (const ViewPointingEvent& e, ToolUtilMovement& movement) {
    if (e.primaryButton () == false) {
      this->updateBrushAndCursorByIntersection (e, false);
      return false;
    }
    else if (this->brush.hasPosition ()) {
      const glm::vec3 oldBrushPos = this->brush.position ();

      if ( movement.move (e, false)
        && this->brush.updatePointOfAction ( movement.position ()
                                           , movement.position () - oldBrushPos ) )
      {
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

  void registerSecondarySlider (ViewDoubleSlider& slider) {
    this->secondarySlider = &slider;
  }

  void setRelativeRadius (float distance) {
    const Camera& cam    = this->self->state ().camera ();
    const float   factor = this->radiusEdit.doubleValue ();
    const float   radius = cam.toWorld (float (cam.resolution ().x) * factor, distance);

    this->absoluteRadius = false;
    this->cursor.radius (radius);
    this->brush.radius (radius);
  }

  void setRelativeRadius () {
    this->setRelativeRadius (glm::distance ( this->cursor.position ()
                                           , this->self->state ().camera ().position () ));
  }

  void setAbsoluteRadius () {
    const float max    = this->self->config ().get <float> ("editor/tool/sculpt/max-absolute-radius");
    const float factor = this->radiusEdit.doubleValue ();

    this->absoluteRadius = true;
    this->brush.radius (factor * max);
    this->cursor.radius (factor * max);
  }
};

DELEGATE_BIG2_BASE (ToolSculpt, (State& s, const char* k), (this), Tool, (s, k))
GETTER          (SculptBrush&, ToolSculpt, brush)
GETTER          (ViewCursor& , ToolSculpt, cursor)
DELEGATE2_CONST (void        , ToolSculpt, addDefaultToolTip, ViewToolTip&, bool)
DELEGATE2_CONST (void        , ToolSculpt, addSecSliderWheelToolTip, ViewToolTip&, const QString&)
DELEGATE        (void        , ToolSculpt, sculpt)
DELEGATE3       (bool        , ToolSculpt, carvelikeStroke, const ViewPointingEvent&, bool, const std::function <void ()>*)
DELEGATE2       (bool        , ToolSculpt, initializeDraglikeStroke, const ViewPointingEvent&, ToolUtilMovement&)
DELEGATE2       (bool        , ToolSculpt, draglikeStroke, const ViewPointingEvent&, ToolUtilMovement&)
DELEGATE1       (void        , ToolSculpt, registerSecondarySlider, ViewDoubleSlider&)
DELEGATE        (ToolResponse, ToolSculpt, runInitialize)
DELEGATE_CONST  (void        , ToolSculpt, runRender)
DELEGATE1       (ToolResponse, ToolSculpt, runPointingEvent, const ViewPointingEvent&)
DELEGATE1       (ToolResponse, ToolSculpt, runWheelEvent, const QWheelEvent&)
DELEGATE1       (ToolResponse, ToolSculpt, runCursorUpdate, const glm::ivec2&)
DELEGATE        (void        , ToolSculpt, runFromConfig)
