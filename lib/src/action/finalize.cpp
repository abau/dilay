#include "action/finalize.hpp"
#include "affected-faces.hpp"
#include "octree.hpp"
#include "partial-action/collapse-face.hpp"
#include "primitive/triangle.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

void Action :: finalize (WingedMesh& mesh, AffectedFaces& affectedFaces) {
  // realign
  for (FacePtrSet::const_iterator it = affectedFaces.faces ().begin (); it != affectedFaces.faces ().end (); ) {
    WingedFace& face = **it;
    it = affectedFaces.removeCommited (it);
    affectedFaces.insert (mesh.realignFace (face, face.triangle (mesh)));
  }
  affectedFaces.commit ();

  while (mesh.octree ().numDegeneratedFaces () > 0) {
    // collapse degenerated faces
    WingedFace* degenerated = nullptr;
    while ((degenerated = mesh.octree ().someDegeneratedFace ()) != nullptr) {
      PartialAction::collapseFace (mesh, *degenerated, affectedFaces);
    }

    FacePtrSet affectedByCollapse = affectedFaces.uncommitedFaces ();
    for (WingedFace* f : affectedByCollapse) {
      affectedFaces.remove (*f);
      affectedFaces.insert (mesh.realignFace (*f, f->triangle (mesh)));
    }
    affectedFaces.commit ();
  }

  // write normals
  for (WingedVertex* v : affectedFaces.toVertexSet ()) {
    v->writeInterpolatedNormal (mesh);
  }

  mesh.bufferData ();

  assert (mesh.octree ().numDegeneratedFaces () == 0);
}
