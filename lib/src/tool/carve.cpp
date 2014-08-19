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
    , brush (0.1f, 0.01f, 0.03f)
  {}

  static QString toolName () {
    return QObject::tr ("Carve");
  }

  bool getIntersection (const glm::ivec2& mouse, WingedMesh*& mesh, glm::vec3& position) {
    PrimRay                ray   = State::camera ().ray (mouse);
    Scene&                 scene = State::scene ();
    WingedFaceIntersection sceneIntersection;
    Intersection           cacheIntersection;

    bool isCacheIntersection  = this->carveCache.intersects (ray, cacheIntersection);
    bool isSceneIntersection = scene.intersects (ray, sceneIntersection);

    if (isSceneIntersection && scene.selection ().hasMajor (sceneIntersection.mesh ().id ())) {
      if (isCacheIntersection) {
        assert (this->carveCache.meshCache ());     // fix corner cases (other meshes)

        mesh     = this->carveCache.meshCache ();
        position = cacheIntersection.position ();
      }
      else {
        this->carveCache.meshCache (&sceneIntersection.mesh ());
        mesh     = &sceneIntersection.mesh    ();
        position = sceneIntersection.position ();
      }
      return true;
    }
    else {
      return false;
    }
  }

  ToolResponse runMouseMoveEvent (QMouseEvent& e) {
    if (e.buttons ().testFlag (Qt::LeftButton)) {
      glm::vec3   position;
      WingedMesh* mesh;

      if (this->getIntersection (ViewUtil::toIVec2 (e), mesh, position)) {
        State::history ().add <ActionCarve, WingedMesh> (*mesh)
                         .run (*mesh
                              , position
                              , this->brush
                              , this->carveCache
                              );
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
