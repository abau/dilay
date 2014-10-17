#include <QMouseEvent>
#include <glm/glm.hpp>
#include <unordered_map>
#include "action/carve.hpp"
#include "camera.hpp"
#include "carve-brush.hpp"
#include "cursor.hpp"
#include "history.hpp"
#include "id.hpp"
#include "mesh-type.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
#include "selection.hpp"
#include "state.hpp"
#include "tool/carve.hpp"
#include "view/util.hpp"
#include "view/tool-menu-parameters.hpp"
#include "view/main-window.hpp"
#include "view/gl-widget.hpp"
#include "winged/face-intersection.hpp"
#include "winged/mesh.hpp"

struct ToolCarve::Impl {
  ToolCarve*        self;
  CarveBrush        brush;
  Cursor            cursor;

  Impl (ToolCarve* s) 
    : self   (s) 
    , brush  (0.1f, 0.005f, 0.03f, 0.2f)
  {
    this->cursor.setGeometry (this->brush.width ());
    this->cursor.enable      ();
    this->updateCursor ( this->self->menuParameters ().mainWindow ().glWidget ().cursorPosition ()
                       , false );
  }

  static QString toolName () {
    return QObject::tr ("Carve");
  }

  bool updateCursor (const glm::ivec2& mouse, bool updateBrush) {
    PrimRay                ray   = State::camera ().ray (mouse);
    Scene&                 scene = State::scene ();
    WingedFaceIntersection intersection;

    if (   scene.intersects (ray, intersection) 
        && scene.selection  ().hasMajor (intersection.mesh ().id ())) 
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
    const bool doCarve = e.buttons ().testFlag (Qt::LeftButton);
    if (this->updateCursor (ViewUtil::toIVec2 (e), doCarve)) {
      State::history ().add <ActionCarve, WingedMesh> (this->brush.mesh ())
                       .run (this->brush);
    }
    return ToolResponse::Redraw;
  }
};

DELEGATE_BIG3_BASE ( ToolCarve, (const ViewToolMenuParameters& p)
                   , (this), Tool, (p, toolName ()) )
DELEGATE_STATIC (QString     , ToolCarve, toolName)
DELEGATE        (void        , ToolCarve, runRender)
DELEGATE1       (ToolResponse, ToolCarve, runMouseMoveEvent, QMouseEvent&)
