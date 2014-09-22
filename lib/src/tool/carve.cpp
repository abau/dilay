#include <QMouseEvent>
#include <glm/glm.hpp>
#include <unordered_map>
#include "action/carve.hpp"
#include "camera.hpp"
#include "carve-brush.hpp"
#include "history.hpp"
#include "id.hpp"
#include "mesh-type.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
#include "selection.hpp"
#include "state.hpp"
#include "tool/carve.hpp"
#include "view/util.hpp"
#include "winged/face-intersection.hpp"
#include "winged/mesh.hpp"

struct ToolCarve::Impl {
  ToolCarve*        self;
  CarveBrush        brush;

  Impl (ToolCarve* s) 
    : self  (s) 
    , brush (0.1f, 0.005f, 0.03f, 0.1f)
  {}

  static QString toolName () {
    return QObject::tr ("Carve");
  }

  bool updateBrush (const glm::ivec2& mouse) {
    PrimRay                ray   = State::camera ().ray (mouse);
    Scene&                 scene = State::scene ();
    WingedFaceIntersection intersection;

    if (   scene.intersects (ray, intersection) 
        && scene.selection  ().hasMajor (intersection.mesh ().id ())) 
    {
      return this->brush.updatePosition ( intersection.mesh     ()
                                        , intersection.position () );
    }
    else {
      return false;
    }
  }

  ToolResponse runMouseReleaseEvent (QMouseEvent& e) {
    if (e.button () == Qt::LeftButton) {
      if (this->updateBrush (ViewUtil::toIVec2 (e))) {
        State::history ().add <ActionCarve, WingedMesh> (this->brush.mesh ())
                         .run (this->brush);
        return ToolResponse::Redraw;
      }
    }
    return ToolResponse::None;
  }
};

DELEGATE_BIG3_BASE ( ToolCarve, (const ViewToolMenuParameters& p)
                   , (this), Tool, (p, toolName ()) )
DELEGATE_STATIC (QString     , ToolCarve, toolName)
DELEGATE1       (ToolResponse, ToolCarve, runMouseReleaseEvent, QMouseEvent&)
