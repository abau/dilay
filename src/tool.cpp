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


void subdivisionStep (const FaceIntersection&);

bool Tool :: click (unsigned int x, unsigned int y) {
  WingedMesh& mesh = State :: global ().mesh ();
  Ray         ray  = State :: global ().camera ().getRay (x,y);
  FaceIntersection intersection;

  if (State :: global ().mesh ().intersectRay (ray,intersection)) {
    subdivisionStep (intersection);
    mesh.rebuildIndices ();
    mesh.rebuildNormals ();
    mesh.bufferData ();
    return true;
  }
  return false;
}

//FaceSet collectFaces (const FaceIntersection&);

void subdivisionStep (const FaceIntersection& intersection) {
  WingedMesh& mesh  = State :: global ().mesh ();
  /*FaceSet     faces = collectFaces (intersection);

  while (! faces.empty ()) {
    SubdivButterfly :: subdivide ( mesh , faces );
    faces = collectFaces (intersection);
  }
  SubdivButterfly :: subdiv ( mesh , { intersection.face () } );
  */
  FaceSet foo = FaceSet ({intersection.face ()});
  SubdivButterfly :: subdivide ( mesh , foo );
}

/*
FaceSet collectFaces (const FaceIntersection& intersection) {
  Sphere sphere (intersection.position (), State :: global ().cursor ().radius ());
  WingedMesh& mesh = State :: global ().mesh ();
  FaceSet     faces;

  for (FACE_ITERATOR (it,mesh)) {
    if (   ( it->longestEdge (mesh)->length (mesh) > 0.1f)
        && ( IntersectionUtil :: intersects (sphere,mesh,*it))) {
      faces.insert (it);
    }
  }
  return faces;
}
*/
