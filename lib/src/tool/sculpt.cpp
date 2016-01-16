/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCheckBox>
#include <QFrame>
#include <QMouseEvent>
#include <QPushButton>
#include <QWheelEvent>
#include "action/sculpt.hpp"
#include "cache.hpp"
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
#include "view/properties.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"
#include "winged/face-intersection.hpp"

struct ToolSculpt::Impl {
  ToolSculpt*       self;
  SculptBrush       brush;
  ViewCursor        cursor;
  CacheProxy        commonCache;
  ViewDoubleSlider& radiusEdit;

  Impl (ToolSculpt* s) 
    : self        (s) 
    , commonCache (this->self->cache ("sculpt"))
    , radiusEdit  (ViewUtil::slider  (2, 0.01f, 0.01f, 2.0f, 3))
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

    this->brush.radius          (cCache.get <float> ("radius"   , 0.2f));
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
    ViewTwoColumnGrid& properties = this->self->properties ().body ();

    this->radiusEdit.setDoubleValue (this->brush.radius ());
    ViewUtil::connect (this->radiusEdit, [this] (float r) {
      this->brush.radius (r);
      this->cursor.radius (r);
      this->commonCache.set ("radius", r);
    });
    properties.addStacked (QObject::tr ("Radius"), this->radiusEdit);

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

  ToolResponse runMouseMoveEvent (const QMouseEvent& e) {
    this->self->runSculptMouseMoveEvent (e);
    return ToolResponse::Redraw;
  }

  ToolResponse runMousePressEvent (const QMouseEvent& e) {
    this->self->snapshotWingedMeshes ();

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
    return ToolResponse::Redraw;
  }

  ToolResponse runWheelEvent (const QWheelEvent& e) {
    if (e.orientation () == Qt::Vertical && e.modifiers () == Qt::ShiftModifier) {
      if (e.delta () > 0) {
        this->radiusEdit.setIntValue ( this->radiusEdit.intValue ()
                                     + this->radiusEdit.intSingleStep () );
      }
      else if (e.delta () < 0) {
        this->radiusEdit.setIntValue ( this->radiusEdit.intValue ()
                                     - this->radiusEdit.intSingleStep () );
      }
    }
    return ToolResponse::Redraw;
  }

  void addDefaultToolTip (ViewToolTip& toolTip, bool hasInvertedMode) {
    toolTip.add (ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to sculpt"));

    if (hasInvertedMode) {
      toolTip.add ( ViewToolTip::MouseEvent::Left
                  , ViewToolTip::Modifier::Shift, QObject::tr ("Drag to sculpt inverted"));
    }
  }

  void sculpt () {
    Action::sculpt (this->brush);
    if (this->self->hasMirror ()) {
      this->brush.mirror (this->self->mirror ().plane ());
      Action::sculpt (this->brush);
      this->brush.mirror (this->self->mirror ().plane ());
    }
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

  bool updateBrushAndCursorByIntersection (const QMouseEvent& e, bool useRecentOctree) {
    WingedFaceIntersection intersection;

    if (this->self->intersectsScene (e, intersection)) {
      this->cursor.enable   ();
      this->cursor.position (intersection.position ());

      if (e.button () == Qt::LeftButton || e.buttons () == Qt::LeftButton) {
        this->brush.mesh (&intersection.mesh ());

        if (useRecentOctree) {
          Intersection octreeIntersection;
          if (this->self->intersectsRecentOctree (e, octreeIntersection)) {
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

  bool carvelikeStroke ( const QMouseEvent& e, bool useRecentOctree
                       , const std::function <void ()>* toggle )
  {
    if (this->updateBrushAndCursorByIntersection (e, useRecentOctree)) {
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
};

DELEGATE_BIG2_BASE (ToolSculpt, (State& s, const char* k), (this), Tool, (s, k))
GETTER          (SculptBrush&, ToolSculpt, brush)
GETTER          (ViewCursor& , ToolSculpt, cursor)
DELEGATE2_CONST (void        , ToolSculpt, addDefaultToolTip, ViewToolTip&, bool)
DELEGATE        (void        , ToolSculpt, sculpt)
DELEGATE3       (bool        , ToolSculpt, carvelikeStroke, const QMouseEvent&, bool, const std::function <void ()>*)
DELEGATE2       (bool        , ToolSculpt, initializeDraglikeStroke, const QMouseEvent&, ToolUtilMovement&)
DELEGATE2       (bool        , ToolSculpt, draglikeStroke, const QMouseEvent&, ToolUtilMovement&)
DELEGATE        (ToolResponse, ToolSculpt, runInitialize)
DELEGATE_CONST  (void        , ToolSculpt, runRender)
DELEGATE1       (ToolResponse, ToolSculpt, runMouseMoveEvent, const QMouseEvent&)
DELEGATE1       (ToolResponse, ToolSculpt, runMousePressEvent, const QMouseEvent&)
DELEGATE1       (ToolResponse, ToolSculpt, runMouseReleaseEvent, const QMouseEvent&)
DELEGATE1       (ToolResponse, ToolSculpt, runWheelEvent, const QWheelEvent&)
