#include <QMouseEvent>
#include <glm/glm.hpp>
#include "tool/carve.hpp"
#include "state.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include "history.hpp"
#include "mesh-type.hpp"
#include "action/carve.hpp"
#include "winged/mesh.hpp"
#include "winged/face-intersection.hpp"
#include "view/util.hpp"
#include "primitive/ray.hpp"
#include "selection.hpp"
#include "id.hpp"

struct ToolCarve::Impl {
  ToolCarve*   self;
  WingedMeshes selection;

  Impl (ToolCarve* s) 
    : self (s) 
    , selection (State::scene ().selectedWingedMeshes (MeshType::Freeform))
  {}

  static QString toolName () {
    return QObject::tr ("Carve");
  }

  ToolResponse runMouseMoveEvent (QMouseEvent& e) {
    if (e.buttons ().testFlag (Qt::LeftButton)) {
      WingedFaceIntersection intersection;
      Scene&                 scene = State::scene ();

      if (scene.intersects (State::camera ().ray (ViewUtil::toIVec2 (e)), intersection)) {
        if (scene.selection ().hasMajor (intersection.mesh ().id ())) {
          State::history ().add <ActionCarve, WingedMesh> (intersection.mesh ())
                           .run ( intersection.mesh     ()
                                , intersection.position ()
                                , 0.1f );

          return ToolResponse::Redraw;
        }
      }
      return ToolResponse::None;
    }
    else {
      return ToolResponse::None;
    }
  }
};

DELEGATE_BIG3_BASE ( ToolCarve, (const ViewToolMenuParameters& p)
                   , (this), Tool, (p, toolName ()) )
DELEGATE_STATIC (QString     , ToolCarve, toolName)
DELEGATE1       (ToolResponse, ToolCarve, runMouseMoveEvent, QMouseEvent&)
