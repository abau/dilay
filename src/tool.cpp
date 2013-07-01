#include "tool.hpp"
#include "fwd-winged.hpp"
#include "ray.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "sphere.hpp"
#include "intersection.hpp"
#include "state.hpp"
#include "cursor.hpp"
#include "camera.hpp"
#include "subdivision-butterfly.hpp"
#include "winged-util.hpp"
#include "view-mouse-movement.hpp"

void subdivisionStep (const FaceIntersection&);

bool Tool :: click () {
  WingedMesh& mesh = State :: mesh ();
  Ray         ray  = State :: camera ().getRayInvY (State :: mouseMovement ()
                                                           . position ());
  FaceIntersection intersection;

  State :: mesh ().intersectRay (ray,intersection);
  if (intersection.isIntersection ()) {
    SubdivButterfly :: subdivide (mesh, intersection.face ());
    mesh.rebuildIndices ();
    mesh.rebuildNormals ();
    mesh.bufferData ();
    return true;
  }
  return false;
}
