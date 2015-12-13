/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QButtonGroup>
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
#include "primitive/sphere.hpp"
#include "sketch/mesh.hpp"
#include "sketch/mesh-intersection.hpp"
#include "sketch/path.hpp"
#include "sketch/path-intersection.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "util.hpp"
#include "view/cursor.hpp"
#include "view/double-slider.hpp"
#include "view/properties.hpp"
#include "view/tool-tip.hpp"
#include "view/util.hpp"

namespace {
  int toInt (SketchPathSmoothEffect effect) {
    switch (effect) {
      case SketchPathSmoothEffect::None:           return 0;
      case SketchPathSmoothEffect::Embed:          return 1;
      case SketchPathSmoothEffect::EmbedAndAdjust: return 2;
      case SketchPathSmoothEffect::Pinch:          return 3;
      default: DILAY_IMPOSSIBLE
    }
  }

  SketchPathSmoothEffect toSmoothEffect (int i) {
    switch (i) {
      case 0:  return SketchPathSmoothEffect::None;
      case 1:  return SketchPathSmoothEffect::Embed;
      case 2:  return SketchPathSmoothEffect::EmbedAndAdjust;
      case 3:  return SketchPathSmoothEffect::Pinch;
      default: DILAY_IMPOSSIBLE
    }
  }
}

struct ToolSketchSpheres::Impl {
  ToolSketchSpheres*     self;
  ViewCursor             cursor;
  ViewDoubleSlider&      radiusEdit;
  ViewDoubleSlider&      heightEdit;
  SketchPathSmoothEffect smoothEffect;
  const float            stepWidthFactor;
  glm::vec3              previousPosition;
  SketchMesh*            mesh;

  Impl (ToolSketchSpheres* s)
    : self            (s)
    , radiusEdit      (ViewUtil::slider ( 2, 0.01f, s->cache ().get <float> ("radius", 0.1f)
                                        , 0.3f ))
    , heightEdit      (ViewUtil::slider ( 2, 0.01f, s->cache ().get <float> ("height", 0.2f)
                                        , 0.45f ))
    , smoothEffect    (toSmoothEffect (s->cache ().get <int> 
                        ("smooth-effect", toInt (SketchPathSmoothEffect::Embed))))
    , stepWidthFactor (s->config ().get <float> ("editor/tool/sketch-spheres/step-width-factor"))
    , mesh            (nullptr)
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

    QButtonGroup& smoothEffectEdit = *new QButtonGroup;
    properties.add ( smoothEffectEdit , { QObject::tr ("None")
                                        , QObject::tr ("Embed")
                                        , QObject::tr ("Embed and adjust")
                                        , QObject::tr ("Pinch")
                                        } );
    ViewUtil::connect (smoothEffectEdit, [this] (int id) {
      this->smoothEffect = toSmoothEffect (id);
      this->self->cache ().set ("smooth-effect", id);
    });
    smoothEffectEdit.button (toInt (this->smoothEffect))->click ();
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

  glm::vec3 newSpherePosition (bool considerHeight, const SketchMeshIntersection& intersection) {
    if (considerHeight) {
      return intersection.position () 
           - (intersection.normal () * float (this->radiusEdit.doubleValue ()
                                     * (1.0f - (2.0f * this->heightEdit.doubleValue ()))));
    }
    else {
      return intersection.position ();
    }
  }

  ToolResponse runMouseMoveEvent (const QMouseEvent& e) {
    auto minDistance = [this] (const Intersection& intersection) -> bool {
      const float d = this->radiusEdit.doubleValue () * this->stepWidthFactor;
      return glm::distance (this->previousPosition, intersection.position ()) > d;
    };

    if (e.buttons () == Qt::LeftButton) {
      if (e.modifiers () == Qt::ShiftModifier) {
        SketchPathIntersection intersection;

        if (this->self->intersectsScene (e, intersection)) {
          this->cursor.enable   ();
          this->cursor.position (intersection.position ());

          if (minDistance (intersection)) {
            this->previousPosition = intersection.position ();
            this->mesh             = &intersection.mesh ();

            this->mesh->smoothPath ( intersection.path ()
                                   , PrimSphere ( intersection.position ()
                                                , this->radiusEdit.doubleValue () )
                                   , 1
                                   , this->smoothEffect
                                   , this->self->mirrorDimension () );
          }
        }
      }
      else if (this->mesh) {
        SketchMeshIntersection intersection;
        const unsigned int     numExcludedLastPaths = this->self->hasMirror () ? 2 : 1;
              bool             considerHeight       = true;

        if (this->self->intersectsScene (e, intersection, numExcludedLastPaths) == false) {
          const Camera&   camera = this->self->state ().camera ();
          const PrimRay   ray    = camera.ray (ViewUtil::toIVec2 (e));
          const PrimPlane plane  = PrimPlane ( this->previousPosition
                                             , DimensionUtil::vector (camera.primaryDimension ()) );
          float t;
          if (IntersectionUtil::intersects (ray, plane, &t)) {
            intersection.update (t, ray.pointAt (t), plane.normal (), *this->mesh);
            considerHeight = false;
          }
        }

        if (intersection.isIntersection () && minDistance (intersection)) {
          this->previousPosition = intersection.position ();

          this->mesh->addSphere ( false
                                , intersection.position ()
                                , this->newSpherePosition (considerHeight, intersection)
                                , this->radiusEdit.doubleValue ()
                                , this->self->mirrorDimension () );
        }
      }
    }
    else {
      SketchMeshIntersection intersection;
      if (this->self->intersectsScene (e, intersection)) {
        this->cursor.enable   ();
        this->cursor.position (intersection.position ());
      }
      else {
        this->cursor.disable ();
      }
    }
    return ToolResponse::Redraw;
  }

  ToolResponse runMousePressEvent (const QMouseEvent& e) {
    auto setupOnIntersection = [this] (const SketchMeshIntersection& intersection) {
      this->cursor.enable   ();
      this->cursor.position (intersection.position ());

      this->self->snapshotSketchMeshes ();

      this->mesh             = &intersection.mesh ();
      this->previousPosition = intersection.position ();
    };

    if (e.button () == Qt::LeftButton) {
      if (e.modifiers () == Qt::ShiftModifier) {
        SketchPathIntersection intersection;
        if (this->self->intersectsScene (e, intersection)) {
          setupOnIntersection (intersection);

          this->mesh->smoothPath ( intersection.path ()
                                 , PrimSphere ( intersection.position ()
                                              , this->radiusEdit.doubleValue () )
                                 , 1
                                 , this->smoothEffect
                                 , this->self->mirrorDimension () );
        }
      }
      else {
        SketchMeshIntersection intersection;
        if (this->self->intersectsScene (e, intersection)) {
          setupOnIntersection (intersection);

          this->mesh->addSphere ( true
                                , intersection.position ()
                                , this->newSpherePosition (true, intersection)
                                , this->radiusEdit.doubleValue ()
                                , this->self->mirrorDimension () );
        }
        else {
          this->cursor.disable ();
        }
      }
      return ToolResponse::Redraw;
    }
    else {
      return ToolResponse::None;
    }
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
