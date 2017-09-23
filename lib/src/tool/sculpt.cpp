/* This file is part of Dilay
 * Copyright © 2015-2017 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <QCheckBox>
#include <QFrame>
#include <QWheelEvent>
#include "cache.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "dynamic/mesh-intersection.hpp"
#include "dynamic/mesh.hpp"
#include "history.hpp"
#include "maybe.hpp"
#include "mirror.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "tool/sculpt.hpp"
#include "tool/sculpt/util/action.hpp"
#include "tool/sculpt/util/brush.hpp"
#include "tool/util/movement.hpp"
#include "tool/util/step.hpp"
#include "view/cursor.hpp"
#include "view/double-slider.hpp"
#include "view/pointing-event.hpp"
#include "view/tool-tip.hpp"
#include "view/two-column-grid.hpp"
#include "view/util.hpp"

struct ToolSculpt::Impl
{
  ToolSculpt*       self;
  SculptBrush       brush;
  ViewCursor        cursor;
  CacheProxy        commonCache;
  ViewDoubleSlider& radiusEdit;
  ViewDoubleSlider* secondarySlider;
  bool              absoluteRadius;
  bool              sculpted;
  ToolUtilStep      step;

  Impl (ToolSculpt* s)
    : self (s)
    , commonCache (this->self->cache ("sculpt"))
    , radiusEdit (
        ViewUtil::slider (2, 0.01f, this->commonCache.get<float> ("radius", 0.1f), 1.0f, 3))
    , secondarySlider (nullptr)
    , absoluteRadius (this->commonCache.get<bool> ("absolute-radius", true))
    , sculpted (false)
  {
  }

  ToolResponse runInitialize ()
  {
    this->self->renderMirror (true);

    this->setupBrush ();
    this->setupCursor ();
    this->setupProperties ();
    this->setupToolTip ();

    return ToolResponse::Redraw;
  }

  void setupBrush ()
  {
    this->brush.subdivide (this->commonCache.get<bool> ("subdivide", true));

    if (this->absoluteRadius)
    {
      this->setAbsoluteRadius ();
    }
    else
    {
      this->setRelativeRadius ();
    }
    this->self->runSetupBrush (this->brush);
  }

  void setupCursor ()
  {
    assert (this->brush.radius () > 0.0f);

    DynamicMeshIntersection intersection;
    if (this->self->intersectsScene (this->self->cursorPosition (), intersection))
    {
      this->cursor.enable ();
      this->cursor.position (intersection.position ());
    }
    else
    {
      this->cursor.disable ();
    }
    this->cursor.radius (this->brush.radius ());

    this->self->runSetupCursor (this->cursor);
  }

  void setupProperties ()
  {
    ViewTwoColumnGrid& properties = this->self->properties ();

    ViewUtil::connect (this->radiusEdit, [this](float r) {
      if (this->absoluteRadius)
      {
        this->setAbsoluteRadius ();
      }
      else
      {
        this->setRelativeRadius ();
      }
      this->commonCache.set ("radius", r);
      this->self->updateGlWidget ();
    });
    properties.addStacked (QObject::tr ("Radius"), this->radiusEdit);

    QCheckBox& absRadiusEdit =
      ViewUtil::checkBox (QObject::tr ("Absolute radius"), this->absoluteRadius);
    ViewUtil::connect (absRadiusEdit, [this](bool a) {
      if (a)
      {
        this->setAbsoluteRadius ();
      }
      else
      {
        this->setRelativeRadius ();
      }
      this->commonCache.set ("absolute-radius", a);
      this->self->updateGlWidget ();
    });
    properties.add (absRadiusEdit);

    QCheckBox& subdivEdit =
      ViewUtil::checkBox (QObject::tr ("Subdivide"), this->brush.subdivide ());
    ViewUtil::connect (subdivEdit, [this](bool s) {
      this->brush.subdivide (s);
      this->commonCache.set ("subdivide", s);
    });
    properties.add (subdivEdit);

    this->self->addMirrorProperties (true);
    properties.add (ViewUtil::horizontalLine ());

    this->self->runSetupProperties (properties);
  }

  void setupToolTip ()
  {
    ViewToolTip toolTip;

    this->self->runSetupToolTip (toolTip);
    toolTip.add (ViewToolTip::Event::MouseWheel, ViewToolTip::Modifier::Shift,
                 QObject::tr ("Change radius"));
    toolTip.add (ViewToolTip::Event::MouseRight, ViewToolTip::Modifier::Shift,
                 QObject::tr ("Drag to change radius"));

    this->self->showToolTip (toolTip);
  }

  void runRender () const
  {
    Camera& camera = this->self->state ().camera ();

    if (this->cursor.isEnabled ())
    {
      this->cursor.render (camera);
    }
  }

  ToolResponse runPointingEvent (const ViewPointingEvent& e)
  {
    if (e.moveEvent () && e.rightButton ())
    {
      if (e.modifiers () == Qt::ShiftModifier)
      {
        this->radiusEdit.setIntValue (this->radiusEdit.intValue () + e.delta ().x);
      }
      else if (this->secondarySlider && e.modifiers () == Qt::ControlModifier)
      {
        this->secondarySlider->setIntValue (this->secondarySlider->intValue () + e.delta ().x);
      }
    }
    else if (e.pressEvent () && e.leftButton ())
    {
      this->self->snapshotDynamicMeshes ();
      this->sculpted = false;
    }

    if (this->self->runSculptPointingEvent (e))
    {
      this->sculpted = true;
    }

    if (e.releaseEvent () && e.leftButton ())
    {
      this->brush.resetPointOfAction ();

      if (this->sculpted == false)
      {
        this->self->state ().history ().dropPastSnapshot ();
      }
    }
    return ToolResponse::Redraw;
  }

  ToolResponse runWheelEvent (const QWheelEvent& e)
  {
    auto updateSlider = [&e](ViewDoubleSlider& slider) {
      if (e.delta () > 0)
      {
        slider.setIntValue (slider.intValue () + slider.intSingleStep ());
      }
      else if (e.delta () < 0)
      {
        slider.setIntValue (slider.intValue () - slider.intSingleStep ());
      }
    };

    if (e.orientation () == Qt::Vertical)
    {
      if (e.modifiers () == Qt::ShiftModifier)
      {
        updateSlider (this->radiusEdit);
      }
      else if (this->secondarySlider && e.modifiers () == Qt::ControlModifier)
      {
        updateSlider (*this->secondarySlider);
      }
    }
    return ToolResponse::Redraw;
  }

  ToolResponse runCursorUpdate (const glm::ivec2& pos)
  {
    DynamicMeshIntersection cursorIntersection;
    this->setCursorByIntersection (pos, cursorIntersection);
    return ToolResponse::Redraw;
  }

  void runFromConfig ()
  {
    const Config& config = this->self->config ();

    this->brush.detailFactor (config.get<float> ("editor/tool/sculpt/detail-factor"));
    this->brush.stepWidthFactor (config.get<float> ("editor/tool/sculpt/step-width-factor"));

    this->cursor.color (this->self->config ().get<Color> ("editor/tool/sculpt/cursor-color"));
  }

  void addDefaultToolTip (ViewToolTip& toolTip, bool hasInvertedMode)
  {
    toolTip.add (ViewToolTip::Event::MouseLeft, QObject::tr ("Drag to sculpt"));

    if (hasInvertedMode)
    {
      toolTip.add (ViewToolTip::Event::MouseLeft, ViewToolTip::Modifier::Shift,
                   QObject::tr ("Drag to sculpt inverted"));
    }
  }

  void addSecSliderWheelToolTip (ViewToolTip& toolTip, const QString& label,
                                 const QString& dragLabel)
  {
    toolTip.add (ViewToolTip::Event::MouseWheel, ViewToolTip::Modifier::Ctrl, label);
    toolTip.add (ViewToolTip::Event::MouseRight, ViewToolTip::Modifier::Ctrl, dragLabel);
  }

  void sculpt ()
  {
    assert (this->brush.hasPointOfAction ());

    ToolSculptAction::sculpt (this->brush);
    if (this->self->hasMirror () && this->brush.mesh ().isEmpty () == false)
    {
      this->brush.mirror (this->self->mirror ().plane ());
      ToolSculptAction::sculpt (this->brush);
      this->brush.mirror (this->self->mirror ().plane ());
    }

    if (this->brush.mesh ().isEmpty ())
    {
      this->self->state ().scene ().deleteEmptyMeshes ();
      this->brush.resetPointOfAction ();
    }
  }

  bool setCursorByIntersection (const glm::ivec2& pos, DynamicMeshIntersection& intersection)
  {
    if (this->self->intersectsScene (pos, intersection))
    {
      if (this->absoluteRadius == false)
      {
        this->setRelativeRadius (intersection.distance ());
      }
      this->cursor.enable ();
      this->cursor.position (intersection.position ());
      return true;
    }
    else
    {
      this->cursor.disable ();
      return false;
    }
  }

  bool updateBrushByIntersection (bool useRecentMesh, const glm::vec3& cursorStep)
  {
    const glm::vec3 from = this->self->state ().camera ().position ();
    const PrimRay   ray = PrimRay (from, cursorStep - from);

    DynamicMeshIntersection intersection;

    if (this->self->intersectsScene (ray, intersection))
    {
      if (this->brush.hasPointOfAction () && (&this->brush.mesh () != &intersection.mesh ()))
      {
        this->brush.mesh ().bufferData ();
      }

      if (useRecentMesh)
      {
        Intersection rIntersection;
        if (this->self->intersectsRecentDynamicMesh (ray, rIntersection))
        {
          this->brush.setPointOfAction (intersection.mesh (), rIntersection.position (),
                                        rIntersection.normal ());
        }
        else
        {
          this->brush.mesh ().bufferData ();
          this->brush.resetPointOfAction ();
          return false;
        }
      }
      else
      {
        this->brush.setPointOfAction (intersection.mesh (), intersection.position (),
                                      intersection.normal ());
      }
      return true;
    }
    else
    {
      this->brush.mesh ().bufferData ();
      this->brush.resetPointOfAction ();
      return false;
    }
  }

  bool drawlikeStroke (const ViewPointingEvent& e, bool useRecentMesh,
                       const std::function<void()>* toggle)
  {
    DynamicMeshIntersection cursorIntersection;

    if (this->setCursorByIntersection (e.position (), cursorIntersection) && e.leftButton ())
    {
      SBParameters& parameters = this->brush.parameters<SBParameters> ();
      const float   defaultIntesity = parameters.intensity ();
      const bool    doToggle = toggle && e.modifiers () == Qt::ShiftModifier;

      parameters.intensity (defaultIntesity * e.intensity ());

      if (doToggle)
      {
        (*toggle) ();
      }

      if (this->brush.hasPointOfAction ())
      {
        this->step.stepWidth (this->brush.stepWidth ());
        this->step.step (this->brush.position (), cursorIntersection.position (),
                         [this, useRecentMesh](const glm::vec3& brushStep) {
                           if (this->brush.hasPointOfAction ())
                           {
                             if (this->updateBrushByIntersection (useRecentMesh, brushStep))
                             {
                               this->sculpt ();
                             }
                             return true;
                           }
                           else
                           {
                             return false;
                           }
                         });
      }
      else
      {
        if (this->updateBrushByIntersection (useRecentMesh, cursorIntersection.position ()))
        {
          this->sculpt ();
        }
      }

      if (this->brush.hasPointOfAction ())
      {
        assert (this->brush.mesh ().isEmpty () == false);
        this->brush.mesh ().bufferData ();
      }

      if (doToggle)
      {
        (*toggle) ();
      }
      parameters.intensity (defaultIntesity);
      return true;
    }
    else
    {
      return false;
    }
  }

  bool grablikeStroke (const ViewPointingEvent& e, ToolUtilMovement& movement)
  {
    DynamicMeshIntersection cursorIntersection;

    if (e.pressEvent ())
    {
      if (e.leftButton ())
      {
        if (this->setCursorByIntersection (e.position (), cursorIntersection))
        {
          this->brush.setPointOfAction (cursorIntersection.mesh (), cursorIntersection.position (),
                                        cursorIntersection.normal ());
          this->cursor.disable ();
          movement.resetPosition (cursorIntersection.position ());
          return true;
        }
        else
        {
          this->brush.resetPointOfAction ();
          return false;
        }
      }
      else
      {
        this->cursor.enable ();
        this->brush.resetPointOfAction ();
        return false;
      }
    }
    else
    {
      if (e.leftButton () == false)
      {
        this->setCursorByIntersection (e.position (), cursorIntersection);
        return false;
      }
      else if (this->brush.hasPointOfAction ())
      {
        if (movement.move (e, false))
        {
          this->brush.setPointOfAction (this->brush.mesh (), movement.position (),
                                        this->brush.normal ());
          this->sculpt ();
          if (this->brush.hasPointOfAction ())
          {
            assert (this->brush.mesh ().isEmpty () == false);
            this->brush.mesh ().bufferData ();
          }
          return true;
        }
        else
        {
          return false;
        }
      }
      else
      {
        return false;
      }
    }
  }

  void registerSecondarySlider (ViewDoubleSlider& slider) { this->secondarySlider = &slider; }

  void setRelativeRadius (float distance)
  {
    const Camera& cam = this->self->state ().camera ();
    const float   factor = this->radiusEdit.doubleValue ();
    const float   radius = cam.toWorld (float(cam.resolution ().x) * factor, distance);

    this->absoluteRadius = false;
    this->cursor.radius (radius);
    this->brush.radius (radius);
  }

  void setRelativeRadius ()
  {
    this->setRelativeRadius (
      glm::distance (this->cursor.position (), this->self->state ().camera ().position ()));
  }

  void setAbsoluteRadius ()
  {
    const float max = this->self->config ().get<float> ("editor/tool/sculpt/max-absolute-radius");
    const float factor = this->radiusEdit.doubleValue ();

    this->absoluteRadius = true;
    this->brush.radius (factor * max);
    this->cursor.radius (factor * max);
  }
};

DELEGATE_BIG2_BASE (ToolSculpt, (State & s, const char* k), (this), Tool, (s, k))
GETTER (SculptBrush&, ToolSculpt, brush)
GETTER (ViewCursor&, ToolSculpt, cursor)
DELEGATE2_CONST (void, ToolSculpt, addDefaultToolTip, ViewToolTip&, bool)
DELEGATE3_CONST (void, ToolSculpt, addSecSliderWheelToolTip, ViewToolTip&, const QString&,
                 const QString&)
DELEGATE (void, ToolSculpt, sculpt)
DELEGATE3 (bool, ToolSculpt, drawlikeStroke, const ViewPointingEvent&, bool,
           const std::function<void()>*)
DELEGATE2 (bool, ToolSculpt, grablikeStroke, const ViewPointingEvent&, ToolUtilMovement&)
DELEGATE1 (void, ToolSculpt, registerSecondarySlider, ViewDoubleSlider&)
DELEGATE (ToolResponse, ToolSculpt, runInitialize)
DELEGATE_CONST (void, ToolSculpt, runRender)
DELEGATE1 (ToolResponse, ToolSculpt, runPointingEvent, const ViewPointingEvent&)
DELEGATE1 (ToolResponse, ToolSculpt, runWheelEvent, const QWheelEvent&)
DELEGATE1 (ToolResponse, ToolSculpt, runCursorUpdate, const glm::ivec2&)
DELEGATE (void, ToolSculpt, runFromConfig)
