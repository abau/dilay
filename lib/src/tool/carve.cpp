#include <unordered_map>
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
#include "carve-brush.hpp"

struct ToolCarve::Impl {
  ToolCarve*        self;
  CarveCache        carveCache;
  CarveBrush        brush;

  Impl (ToolCarve* s) 
    : self  (s) 
    , brush (0.1f, 0.01f, 0.03f, 0.1f)
  {}

  static QString toolName () {
    return QObject::tr ("Carve");
  }

  bool updateBrush (const glm::ivec2& mouse) {
    PrimRay                ray   = State::camera ().ray (mouse);
    Scene&                 scene = State::scene ();
    WingedFaceIntersection sceneIntersection;
    Intersection           cacheIntersection;

    bool isCacheIntersection  = this->carveCache.intersects (ray, cacheIntersection);
    bool isSceneIntersection = scene.intersects (ray, sceneIntersection);

    if (isSceneIntersection && scene.selection ().hasMajor (sceneIntersection.mesh ().id ())) {
      if (isCacheIntersection) {
        // TODO: fix corner cases (other meshes)

        return this->brush.updatePosition ( this->brush.mesh ()
                                          , cacheIntersection.position () );
      }
      else {
        return this->brush.updatePosition ( sceneIntersection.mesh     ()
                                          , sceneIntersection.position () );
      }
    }
    else {
      return false;
    }
  }

  ToolResponse runMouseMoveEvent (QMouseEvent& e) {
    if (e.buttons ().testFlag (Qt::LeftButton)) {
      if (this->updateBrush (ViewUtil::toIVec2 (e))) {
        State::history ().add <ActionCarve, WingedMesh> (this->brush.mesh ())
                         .run (this->brush, this->carveCache);
        return ToolResponse::Redraw;
      }
    }
    return ToolResponse::None;
  }

  ToolResponse runMouseReleaseEvent (QMouseEvent& e) { 
    if (e.button () == Qt::LeftButton) {
      this->carveCache.reset ();
    }
    return ToolResponse::None; 
  }
};

DELEGATE_BIG3_BASE ( ToolCarve, (const ViewToolMenuParameters& p)
                   , (this), Tool, (p, toolName ()) )
DELEGATE_STATIC (QString     , ToolCarve, toolName)
DELEGATE1       (ToolResponse, ToolCarve, runMouseMoveEvent, QMouseEvent&)
DELEGATE1       (ToolResponse, ToolCarve, runMouseReleaseEvent, QMouseEvent&)
