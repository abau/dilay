#include "tool/carve.hpp"
#include "primitive/ray.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "primitive/sphere.hpp"
#include "intersection.hpp"
#include "state.hpp"
#include "cursor.hpp"
#include "camera.hpp"
#include "view/mouse-movement.hpp"
#include "history.hpp"
#include "action/carve.hpp"
#include "scene.hpp"
//#include "action/subdivide.hpp"
#include "mesh-type.hpp"

bool ToolCarve :: click () {
  /*
  Ray         ray    = State :: camera ().ray (State :: mouseMovement (). position ());
  Cursor&     cursor = State :: cursor ();
  FaceIntersection intersection;

  State :: scene ().intersectRay (MeshType::FreeForm,ray,intersection);
  if (intersection.isIntersection ()) {
    State :: history ().add <ActionCarve> (intersection.mesh ())
      ->run (intersection.mesh (), intersection.position (), cursor.radius ());
    //State :: history ().add <ActionSubdivide> (mesh)
    //  ->run (mesh, intersection.face ());
    intersection.mesh ().bufferData ();
    return true;
  }
  */
  return false;
}
