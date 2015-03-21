#include <glm/glm.hpp>
#include <limits>
#include <map>
#include "action/new-winged-mesh.hpp"
#include "maybe.hpp"
#include "mesh-definition.hpp"
#include "primitive/triangle.hpp"
#include "render-mode.hpp"
#include "scene.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {

  typedef std::pair <unsigned int,unsigned int> UIPair;
  typedef std::map <UIPair, WingedEdge*>        EdgeMap;

  /** `findOrAddEdge (w,m,i1,i2,f)` searches an edge with indices `(i2,i1)` 
   * (in that order) in `m`.
   * If such an edge exists, `f` becomes its new right face.
   * Otherwise a new edge is added to `w` and `m` with `f` being its left face.
   * The found (resp. created) edge is returned
   */
  WingedEdge& findOrAddEdge ( WingedMesh& mesh, EdgeMap& map
                            , unsigned int index1, unsigned int index2
                            , WingedFace& face)
  {
    auto result = map.find (UIPair (index2, index1));
    if (result == map.end ()) {
      WingedVertex* v1    = mesh.vertex (index1);
      WingedVertex* v2    = mesh.vertex (index2);
      WingedEdge& newEdge = mesh.addEdge ();
        
      map.insert (std::pair <UIPair,WingedEdge*> ( UIPair (index1,index2)
                                                 , &newEdge ));
      newEdge.vertex1  (v1);
      newEdge.vertex2  (v2);
      newEdge.leftFace (&face);

      v1-> edge (&newEdge);
      v2-> edge (&newEdge);
      face.edge (&newEdge);

      return newEdge;
    }
    else {
      WingedEdge* existingEdge = result->second;

      existingEdge->rightFace (&face);
      face.edge (existingEdge);

      return *existingEdge;
    }
  }
}

WingedMesh& Action::newWingedMesh (Scene& scene, const MeshDefinition& def) {
  const WingedMesh* other = scene.someWingedMesh ();
        WingedMesh& mesh  = scene.newWingedMesh ();

  // octree
  glm::vec3 maxVertex (std::numeric_limits <float>::lowest ());
  glm::vec3 minVertex (std::numeric_limits <float>::max    ());

  for (unsigned int i = 0; i < def.numVertices (); i++) {
    maxVertex = glm::max (maxVertex, def.vertex (i));
    minVertex = glm::min (minVertex, def.vertex (i));
  }
  const glm::vec3 center = (maxVertex + minVertex) * glm::vec3 (0.5f);
  const glm::vec3 delta  =  maxVertex - minVertex;
  const float     width  = glm::max (glm::max (delta.x, delta.y), delta.z);

  mesh.setupOctreeRoot (center, width);

  // vertices
  for (unsigned int i = 0; i < def.numVertices (); i++) {
    mesh.addVertex (def.vertex (i));
  }

  // faces & edges
  EdgeMap edgeMap;

  for (unsigned int i = 0; i < def.numFace3 (); i++) {
    unsigned int index1, index2, index3;
    def.face (i, index1, index2, index3);

    WingedFace& f = mesh.addFace (PrimTriangle ( mesh, *mesh.vertex (index1)
                                                     , *mesh.vertex (index2)
                                                     , *mesh.vertex (index3) ));
    WingedEdge& e1 = findOrAddEdge (mesh, edgeMap, index1, index2, f);
    WingedEdge& e2 = findOrAddEdge (mesh, edgeMap, index2, index3, f);
    WingedEdge& e3 = findOrAddEdge (mesh, edgeMap, index3, index1, f);

    e1.predecessor (f, &e3);
    e1.successor   (f, &e2);
    e2.predecessor (f, &e1);
    e2.successor   (f, &e3);
    e3.predecessor (f, &e2);
    e3.successor   (f, &e1);
  }

  // render mode
  if (other) {
    mesh.renderMode () = other->renderMode ();
  }

  // write & buffer
  mesh.writeAllIndices ();
  mesh.writeAllNormals ();
  mesh.bufferData      ();

  return mesh;
}
