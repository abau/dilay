#include "action/finalize.hpp"
#include "affected-faces.hpp"
#include "octree.hpp"
#include "partial-action/collapse-face.hpp"
#include "primitive/triangle.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

void Action :: collapseDegeneratedFaces (WingedMesh& mesh) {
  AffectedFaces affectedFaces;
  Action::collapseDegeneratedFaces (mesh, affectedFaces);
}

void Action :: collapseDegeneratedFaces (WingedMesh& mesh, AffectedFaces& affectedFaces) {
  WingedFace* degenerated = nullptr;
  while ((degenerated = mesh.someDegeneratedFace ()) != nullptr) {
    PartialAction::collapseFace (mesh, *degenerated, affectedFaces);

    FacePtrSet affectedByCollapse = affectedFaces.uncommitedFaces ();
    for (WingedFace* f : affectedByCollapse) {
      f->writeIndices (mesh);
      mesh.realignFace (*f);
    }
    affectedFaces.commit ();
  }
}

void Action :: finalize (WingedMesh& mesh, AffectedFaces& affectedFaces) {
  for (WingedFace* f : affectedFaces.faces ()) {
    mesh.realignFace (*f);
  }
  Action::collapseDegeneratedFaces (mesh, affectedFaces);

  for (WingedVertex* v : affectedFaces.toVertexSet ()) {
    v->writeInterpolatedNormal (mesh);
  }
  mesh.bufferData ();

  assert (mesh.octree ().numDegeneratedFaces () == 0);
}
