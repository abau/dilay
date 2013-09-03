#include <glm/glm.hpp>
#include <list>
#include "tool-carve.hpp"
#include "state.hpp"
#include "view-mouse-movement.hpp"
#include "cursor.hpp"
#include "winged-mesh.hpp"
#include "winged-face.hpp"
#include "ray.hpp"
#include "intersection.hpp"
#include "camera.hpp"
#include "sphere.hpp"
#include "subdivision-butterfly.hpp"
#include "id.hpp"

void collectFaces (WingedMesh&, const Sphere&, std::list <Id>&);

bool ToolCarve :: run () {
  Cursor&       cursor = State :: cursor ();
  WingedMesh&   mesh   = State :: mesh ();
  MouseMovement mm     = State :: mouseMovement ();
  Ray           ray    = State :: camera ().getRay (mm.position ());
  FaceIntersection intersection;

  State :: mesh ().intersectRay (ray,intersection);
  if (intersection.isIntersection ()) {
    std::list <Id> ids;
    Sphere         sphere (intersection.position (), cursor.radius ());

    //mesh.intersectSphere (sphere, ids);
    //SubdivButterfly :: subdivide (mesh, ids);
    SubdivButterfly :: subdivide (mesh, intersection.face ());
    mesh.bufferData ();
    return true;
  }
  return false;
}
