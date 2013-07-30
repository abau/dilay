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
#include "octree.hpp"
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

    collectFaces (mesh, Sphere (intersection.position (), cursor.radius ()), ids);

    SubdivButterfly :: subdivide (mesh, ids);
    mesh.rebuildIndices ();
    mesh.rebuildNormals ();
    mesh.bufferData ();
    return true;
  }
  return false;
}

void collectFaces (WingedMesh& mesh, const Sphere& sphere, std::list <Id>& ids) {
  for (OctreeNodeIterator nIt = mesh.octreeNodeIterator (); nIt.isValid (); nIt.next ()) {
    OctreeNode& node = nIt.element ();
    if (IntersectionUtil :: intersects (sphere, node)) {
      for (auto fIt = node.faceIterator (); fIt.isValid (); fIt.next ()) {
        WingedFace& face = fIt.element ();
        if (IntersectionUtil :: intersects (sphere, mesh, face)) {
          ids.push_back (face.id ());
        }
      }
    }
  }
}
