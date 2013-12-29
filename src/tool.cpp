#include "tool.hpp"
#include "ray.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "sphere.hpp"
#include "intersection.hpp"
#include "state.hpp"
#include "cursor.hpp"
#include "camera.hpp"
#include "winged-util.hpp"
#include "view-mouse-movement.hpp"
#include "history.hpp"
#include "action/carve.hpp"

#include "action/subdivide.hpp" // delete this

bool Tool :: click () {
  WingedMesh& mesh   = State :: mesh ();
  Ray         ray    = State :: camera ().getRay (State :: mouseMovement (). position ());
  Cursor&     cursor = State :: cursor ();
  FaceIntersection intersection;

  State :: mesh ().intersectRay (ray,intersection);
  if (intersection.isIntersection ()) {
    State :: history ().add <ActionCarve> (mesh)
      ->run (mesh, intersection.position (), cursor.radius ());
    //State :: history ().add <ActionSubdivide> (mesh)
    //  ->run (mesh, intersection.face ());
    mesh.bufferData ();
    return true;
  }
  return false;
}
