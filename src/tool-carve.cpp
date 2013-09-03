#include <glm/glm.hpp>
#include <list>
#include "tool-carve.hpp"
#include "state.hpp"
#include "view-mouse-movement.hpp"
#include "cursor.hpp"
#include "winged-mesh.hpp"
#include "winged-face.hpp"
#include "winged-edge.hpp"
#include "winged-vertex.hpp"
#include "ray.hpp"
#include "intersection.hpp"
#include "camera.hpp"
#include "sphere.hpp"
#include "subdivision-butterfly.hpp"
#include "id.hpp"
#include "adjacent-iterator.hpp"

#include <iostream>

void refine (const Sphere&, float, std::unordered_set <WingedVertex*>&);

bool ToolCarve :: run () {
  const float   size   = 0.1;
  Cursor&       cursor = State :: cursor ();
  WingedMesh&   mesh   = State :: mesh ();
  MouseMovement mm     = State :: mouseMovement ();
  Ray           ray    = State :: camera ().getRay (mm.position ());
  FaceIntersection intersection;

  State :: mesh ().intersectRay (ray,intersection);
  if (intersection.isIntersection ()) {
    Sphere sphere (intersection.position (), cursor.radius ());
    std::unordered_set <WingedVertex*> vertices;

    refine (sphere,size,vertices);
    mesh.bufferData ();
    return true;
  }
  return false;
}

void refine ( const Sphere& sphere, float brushSize
            , std::unordered_set <WingedVertex*>& vertices) {
  WingedMesh& mesh = State :: mesh ();
  vertices.clear ();
  mesh.intersectSphere (sphere, vertices);

  std::unordered_set <WingedEdge*> edges;
  for (WingedVertex* v : vertices) {
    for (auto it = v->adjacentEdgeIterator (true); it.isValid (); it.next ()) {
      WingedEdge& e = it.element ();
      if (e.lengthSqr (mesh) > brushSize * brushSize) {
        edges.insert (&e);
      }
    }
  }

  if (edges.size () == 0)
    return;
  else {
    for (WingedEdge* e : edges) {
      SubdivButterfly::subdivide (mesh,*e,brushSize);
    }
    refine (sphere,brushSize,vertices);
  }
}
