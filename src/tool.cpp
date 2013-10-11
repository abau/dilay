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
#include "action/subdivide.hpp"

bool Tool :: click () {
  WingedMesh& mesh = State :: mesh ();
  Ray         ray  = State :: camera ().getRay (State :: mouseMovement (). position ());
  FaceIntersection intersection;

  State :: mesh ().intersectRay (ray,intersection);
  if (intersection.isIntersection ()) {
    State :: history ().add <ActionSubdivide> ()->run (mesh, intersection.face ());
    //mesh.rebuildIndices ();
    //mesh.rebuildNormals ();
    mesh.bufferData ();
    return true;
  }
  return false;
}
