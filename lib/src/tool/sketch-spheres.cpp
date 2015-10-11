/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCheckBox>
#include <QFrame>
#include <QMouseEvent>
#include <QPushButton>
#include "cache.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "dimension.hpp"
#include "scene.hpp"
#include "primitive/plane.hpp"
#include "primitive/ray.hpp"
#include "sketch/mesh.hpp"
#include "sketch/mesh-intersection.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "util.hpp"
#include "view/cursor.hpp"
#include "view/double-slider.hpp"
#include "view/properties.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"

struct ToolSketchSpheres::Impl {
  ToolSketchSpheres* self;
  ViewCursor         cursor;
  ViewDoubleSlider&  radiusEdit;
  ViewDoubleSlider&  heightEdit;
  ViewDoubleSlider&  intensityEdit;
  ViewDoubleSlider&  falloffEdit;
  const float        stepWidthFactor;
  glm::vec3          previousPosition;
  SketchMesh*        mesh;

  Impl (ToolSketchSpheres* s)
    : self            (s)
    , radiusEdit      (ViewUtil::slider ( 2, 0.01f, s->cache ().get <float> ("radius", 0.1f)
                                        , 0.3f ))
    , heightEdit      (ViewUtil::slider ( 2, 0.01f, s->cache ().get <float> ("height", 0.2f)
                                        , 0.5f ))
    , intensityEdit   (ViewUtil::slider ( 2, 0.01f, s->cache ().get <float> ("intensity", 0.05f)
                                        , 0.1f ))
    , falloffEdit     (ViewUtil::slider ( 2, 1.5f, s->cache ().get <float> ("falloff", 2.0f)
                                        , 4.0f ))
    , stepWidthFactor (s->config ().get <float> ("editor/tool/sketch-spheres/step-width-factor"))
  {}

  void setupProperties () {
    ViewPropertiesPart& properties = this->self->properties ().body ();

    QPushButton& syncButton = ViewUtil::pushButton (QObject::tr ("Sync"));
    ViewUtil::connect (syncButton, [this] () {
      this->self->mirrorSketchMeshes ();
      this->self->updateGlWidget ();
    });
    syncButton.setEnabled (this->self->hasMirror ());

    QCheckBox& mirrorEdit = ViewUtil::checkBox ( QObject::tr ("Mirror")
                                               , this->self->hasMirror () );
    ViewUtil::connect (mirrorEdit, [this, &syncButton] (bool m) {
      this->self->mirror (m);
      syncButton.setEnabled (m);
    });
    properties.add (mirrorEdit, syncButton);

    ViewUtil::connect (this->radiusEdit, [this] (float r) {
      this->cursor.radius (r);
      this->self->cache ().set ("radius", r);
    });
    properties.addStacked (QObject::tr ("Radius"), this->radiusEdit);

    ViewUtil::connect (this->heightEdit, [this] (float d) {
      this->self->cache ().set ("height", d);
    });
    properties.addStacked (QObject::tr ("Height"), this->heightEdit);

    ViewUtil::connect (this->intensityEdit, [this] (float i) {
      this->self->cache ().set ("intensity", i);
    });
    properties.addStacked (QObject::tr ("Scaling-Intensity"), this->intensityEdit);

    ViewUtil::connect (this->falloffEdit, [this] (float f) {
      this->self->cache ().set ("falloff", f);
    });
    properties.addStacked (QObject::tr ("Scaling-Falloff"), this->falloffEdit);
  }

  void setupToolTip () {
    ViewToolTip toolTip;
    toolTip.add ( ViewToolTip::MouseEvent::Left, QObject::tr ("Drag to sketch"));
    toolTip.add ( ViewToolTip::MouseEvent::Left, ViewToolTip::Modifier::Shift
                , QObject::tr ("Drag to shrink"));
    toolTip.add ( ViewToolTip::MouseEvent::Left, ViewToolTip::Modifier::Ctrl
                , QObject::tr ("Drag to enlarge"));
    toolTip.add ( ViewToolTip::MouseEvent::Wheel, ViewToolTip::Modifier::Shift
                , QObject::tr ("Change radius") );
    this->self->showToolTip (toolTip);
  }

  void setupCursor () {
    this->cursor.disable ();
    this->cursor.radius  (this->radiusEdit.doubleValue ());
    this->cursor.color   (this->self->config ().get <Color>
                           ("editor/tool/sketch-spheres/cursor-color"));
  }

  ToolResponse runInitialize () {
    this->self->renderMirror (false);

    this->setupProperties ();
    this->setupToolTip    ();
    this->setupCursor     ();

    return ToolResponse::Redraw;
  }

  void runRender () const {
    Camera& camera = this->self->state ().camera ();

    if (this->cursor.isEnabled ()) {
      this->cursor.render (camera);
    }
  }

  bool intersectsSketch ( const QMouseEvent& e, SketchMeshIntersection& intersection
                        , unsigned int numExcludedLastPaths )
  {
    if (this->self->intersectsScene (e, intersection, numExcludedLastPaths)) {
      return true;
    }
    else if (this->mesh) {
      const Camera&   camera = this->self->state ().camera ();
      const PrimRay   ray    = camera.ray (ViewUtil::toIVec2 (e));
      const PrimPlane plane  = PrimPlane ( this->previousPosition
                                         , DimensionUtil::vector (camera.primaryDimension ()) );
      float t;
      if (IntersectionUtil::intersects (ray, plane, &t)) {
        intersection.update (t, ray.pointAt (t), plane.normal (), *this->mesh);
      }
    }
    return intersection.isIntersection ();
  }

  ToolResponse runMouseEvent (const QMouseEvent& e, bool startNewPath) {
    const unsigned int numExcludedLastPaths = startNewPath
                                            ? 0
                                            : (this->self->hasMirror () ? 2 : 1);
    SketchMeshIntersection intersection;
    if (this->intersectsSketch (e, intersection, numExcludedLastPaths)) {
      this->cursor.enable   ();
      this->cursor.position (intersection.position ());

      if (startNewPath || this->mesh == &intersection.mesh ()) {
        const float radius      = this->radiusEdit.doubleValue ();
        const float height      = this->heightEdit.doubleValue ();
        const float minDistance = radius * this->stepWidthFactor;
        const bool  hasDistance = startNewPath ? true
                                : glm::distance (this->previousPosition, intersection.position ())
                                > minDistance;
        
        if (hasDistance && (e.button () == Qt::LeftButton || e.buttons () == Qt::LeftButton)) {
          this->mesh             = &intersection.mesh ();
          this->previousPosition = intersection.position ();

          if (e.modifiers () == Qt::ShiftModifier) {
            intersection.mesh ().smoothPaths ( intersection.position ()
                                             , radius
                                             , 1
                                             , this->self->mirrorDimension () );
          }
          else {
            // TODO add more spheres if distance is big
            const glm::vec3 spherePos = intersection.position () 
                                      - (intersection.normal () * radius 
                                                                * (1.0f - (2.0f * height)));

            intersection.mesh ().addSphere ( startNewPath, spherePos, radius
                                           , this->self->mirrorDimension () );
          }
        }
      }
    }
    else {
      this->cursor.disable ();
    }
    return ToolResponse::Redraw;
  }

  ToolResponse runMouseMoveEvent (const QMouseEvent& e) {
    return this->runMouseEvent (e, false);
  }

  ToolResponse runMousePressEvent (const QMouseEvent& e) {
    this->self->snapshotSketchMeshes ();

    return this->runMouseEvent (e, true);
  }

  ToolResponse runMouseReleaseEvent (const QMouseEvent&) {
    this->mesh = nullptr;
    return ToolResponse::None;
  }

  ToolResponse runWheelEvent (const QWheelEvent& e) {
    if (e.orientation () == Qt::Vertical && e.modifiers () == Qt::ShiftModifier) {
      if (e.delta () > 0) {
        this->radiusEdit.setDoubleValue ( this->radiusEdit.doubleValue ()
                                        + this->radiusEdit.doubleSingleStep () );
      }
      else if (e.delta () < 0) {
        this->radiusEdit.setDoubleValue ( this->radiusEdit.doubleValue ()
                                        - this->radiusEdit.doubleSingleStep () );
      }
    }
    return ToolResponse::Redraw;
  }
};

DELEGATE_TOOL                         (ToolSketchSpheres)
DELEGATE_TOOL_RUN_INITIALIZE          (ToolSketchSpheres)
DELEGATE_TOOL_RUN_RENDER              (ToolSketchSpheres)
DELEGATE_TOOL_RUN_MOUSE_MOVE_EVENT    (ToolSketchSpheres)
DELEGATE_TOOL_RUN_MOUSE_PRESS_EVENT   (ToolSketchSpheres)
DELEGATE_TOOL_RUN_MOUSE_RELEASE_EVENT (ToolSketchSpheres)
DELEGATE_TOOL_RUN_MOUSE_WHEEL_EVENT   (ToolSketchSpheres)
