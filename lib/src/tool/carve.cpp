#include <QDoubleSpinBox>
#include <QMouseEvent>
#include <glm/glm.hpp>
#include <unordered_map>
#include "action/carve.hpp"
#include "camera.hpp"
#include "carve-brush.hpp"
#include "config.hpp"
#include "cursor.hpp"
#include "history.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
#include "selection.hpp"
#include "state.hpp"
#include "tools.hpp"
#include "view/util.hpp"
#include "view/tool/menu-parameters.hpp"
#include "view/main-window.hpp"
#include "view/gl-widget.hpp"
#include "view/properties.hpp"
#include "view/util.hpp"
#include "winged/face-intersection.hpp"
#include "winged/mesh.hpp"

struct ToolCarve::Impl {
  ToolCarve* self;
  CarveBrush brush;
  Cursor     cursor;

  Impl (ToolCarve* s) 
    : self   (s) 
    , brush  ( this->self->config ().get <float> ("radius"           , 10.0f)
             , this->self->config ().get <float> ("detail"           ,  0.6f)
             , this->self->config ().get <float> ("intensity-factor" ,  0.1f)
             , this->self->config ().get <float> ("step-width-factor",  0.3f) )
    , cursor (this->brush.radius ())
  {
    QDoubleSpinBox& radiusEdit = ViewUtil::spinBox (0.01f, this->brush.radius (), 1000.0f, 1.0f);
    ViewUtil::connect (radiusEdit, [this] (float r) {
      this->brush .radius         (r);
      this->cursor.radius         (r);
      this->cursor.updateGeometry ();
      this->self->updateGlWidget  ();
      this->self->config ().cache ("radius", r);
    });
    this->self->properties ().addWidget (QObject::tr ("Radius"), radiusEdit);

    QDoubleSpinBox& detailEdit = ViewUtil::spinBox (0.01f, this->brush.detail (), 0.95f, 0.1f);
    ViewUtil::connect (detailEdit, [this] (float h) {
      this->brush.detail (h);
      this->self->config ().cache ("detail", h);
    });
    this->self->properties ().addWidget (QObject::tr ("Detail"), detailEdit);

    QDoubleSpinBox& intensityEdit = ViewUtil::spinBox ( 0.0f, this->brush.intensityFactor ()
                                                      , 1000.0f, 0.1f );
    ViewUtil::connect (intensityEdit, [this] (float d) {
      this->brush.intensityFactor (d);
      this->self->config ().cache ("intensity", d);
    });
    this->self->properties ().addWidget (QObject::tr ("Intensity"), intensityEdit);

    QDoubleSpinBox& stepEdit = ViewUtil::spinBox ( 0.01f, this->brush.stepWidthFactor ()
                                                 , 1000.0f, 0.1f );
    ViewUtil::connect (stepEdit, [this] (float s) {
      this->brush.stepWidthFactor (s);
      this->self->config ().cache ("step-width-factor", s);
    });
    this->self->properties ().addWidget (QObject::tr ("Step width"), stepEdit);

    this->updateCursor (State::mainWindow ().glWidget ().cursorPosition (), false);
  }

  bool updateCursor (const glm::ivec2& mouse, bool updateBrush) {
    PrimRay                ray   = State::camera ().ray (mouse);
    Scene&                 scene = State::scene ();
    WingedFaceIntersection intersection;

    if (   scene.intersects (ray, intersection) 
        && scene.selection  ().hasMajor (intersection.mesh ().index ())) 
    {
      this->cursor.position (intersection.position ());
      this->cursor.normal   (intersection.normal   ());

      return updateBrush 
        ? this->brush.updatePosition ( intersection.mesh     ()
                                     , intersection.position () )
        : false;
    }
    else {
      return false;
    }
  }

  void runRender () {
    this->cursor.render ();
  }

  ToolResponse runMouseMoveEvent (QMouseEvent& e) {
    const bool doCarve = e.buttons () == Qt::LeftButton;
    if (this->updateCursor (ViewUtil::toIVec2 (e), doCarve)) {
      State::history ().add <ActionCarve, WingedMesh> (this->brush.mesh ())
                       .run (this->brush);
    }
    return ToolResponse::Redraw;
  }
};

DELEGATE_TOOL                      (ToolCarve)
DELEGATE_TOOL_RUN_RENDER           (ToolCarve)
DELEGATE_TOOL_RUN_MOUSE_MOVE_EVENT (ToolCarve)
