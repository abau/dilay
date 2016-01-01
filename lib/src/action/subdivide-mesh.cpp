/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <glm/glm.hpp>
#include "action/subdivide-mesh.hpp"
#include "affected-faces.hpp"
#include "partial-action/insert-edge-vertex.hpp"
#include "partial-action/triangulate-6-gon.hpp"
#include "subdivision-butterfly.hpp"
#include "winged/mesh.hpp"

void Action::subdivideMesh (WingedMesh& mesh) {
  AffectedFaces affected;
  mesh.forEachFace ([&affected] (WingedFace& f) {
    affected.insert (f);
  });
  affected.commit ();

  // subdivide edges
  for (WingedEdge* e : affected.toEdgeVec ()) {
    const glm::vec3 p = SubdivisionButterfly::subdivideEdge (mesh, *e);
    PartialAction::insertEdgeVertex (mesh, *e , p);
  }

  // triangulate faces
  for (WingedFace* f : affected.faces ()) {
    PartialAction::triangulate6Gon (mesh, *f, affected);
  }

  mesh.writeAllIndices ();
  mesh.writeAllNormals ();
  mesh.realignAllFaces ();
  mesh.bufferData      ();
}
