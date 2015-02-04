#include <unordered_set>
#include "action/on-post-processed-winged-mesh.hpp"
#include "action/unit/on.hpp"
#include "affected-faces.hpp"
#include "bitset.hpp"
#include "octree.hpp"
#include "partial-action/collapse-face.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "primitive/triangle.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"

struct ActionOnPostProcessedWMesh :: Impl {
  std::unordered_set <unsigned int> realignedFaceIndices;
  Bitset <char>                     flags;

  Impl () {
    this->doRealignAllFaces (false);
    this->doWriteAllIndices (false);
    this->doWriteAllNormals (false);
    this->doBufferData      (false);
  }

  bool doRealignAllFaces () const { return this->flags.get (0);   }
  void doRealignAllFaces (bool v) {        this->flags.set (0,v); }
  bool doWriteAllNormals () const { return this->flags.get (1);   }
  void doWriteAllNormals (bool v) {        this->flags.set (1,v); }
  bool doWriteAllIndices () const { return this->flags.get (2);   }
  void doWriteAllIndices (bool v) {        this->flags.set (2,v); }
  bool doBufferData      () const { return this->flags.get (3);   }
  void doBufferData      (bool v) {        this->flags.set (3,v); }

  WingedFace& realignFace (WingedMesh& mesh, WingedFace& face) {
    bool        sameNode  = false;
    WingedFace& realigned = mesh.realignFace ( face
                                             , face.triangle (mesh)
                                             , &sameNode );
    if (sameNode == false) {
      this->realignedFaceIndices.insert (realigned.index ());
    }
    return realigned;
  }

  void realignAllFaces (WingedMesh& mesh) {
    this->doRealignAllFaces (true);
    mesh.realignAllFaces    ();
  }

  void writeAllNormals (WingedMesh& mesh) {
    this->doWriteAllNormals (true);
    mesh.writeAllNormals    ();
  }

  void writeAllIndices (WingedMesh& mesh) {
    this->doWriteAllIndices (true);
    mesh.writeAllIndices    ();
  }

  void bufferData (WingedMesh& mesh) {
    this->doBufferData (true);
    mesh.bufferData    ();
  }

  void postProcess (WingedMesh& mesh) const {
    if (this->doRealignAllFaces ()) {
      mesh.realignAllFaces ();
    }
    else {
      for (unsigned int index : this->realignedFaceIndices) {
        WingedFace* face = mesh.face (index);
        if (face) {
          mesh.realignFace (*face, face->triangle (mesh), nullptr);
        }
      }
    }
    if (this->doWriteAllNormals ()) {
      mesh.writeAllNormals ();
    }
    if (this->doWriteAllIndices ()) {
      mesh.writeAllIndices ();
    }
    if (this->doBufferData ()) {
      mesh.bufferData ();
    }
  }

  void finalize ( WingedMesh& mesh, AffectedFaces& affectedFaces
                , ActionUnitOn <WingedMesh>& actions )
  {
    // realign
    for (auto it = affectedFaces.faces ().begin (); it != affectedFaces.faces ().end (); ) {
      WingedFace& face = **it;
      ++it;
      affectedFaces.remove (face);
      affectedFaces.insert (this->realignFace (mesh, face));
    }
    affectedFaces.commit ();

    while (mesh.octree ().numDegeneratedFaces () > 0) {
      // collapse degenerated faces
      WingedFace* degenerated = nullptr;
      while ((degenerated = mesh.octree ().someDegeneratedFace ()) != nullptr) {
        actions.add <PACollapseFace> ().run (mesh, *degenerated, affectedFaces);
      }

      FacePtrSet affectedByCollapse = affectedFaces.uncommitedFaces ();
      for (WingedFace* f : affectedByCollapse) {
        affectedFaces.remove (*f);
        affectedFaces.insert (this->realignFace (mesh, *f));
      }
      affectedFaces.commit ();
    }

    // write normals
    for (WingedVertex* v : affectedFaces.toVertexSet ()) {
      actions.add <PAModifyWVertex> ().writeInterpolatedNormal (mesh,*v);
    }

    this->bufferData (mesh);
    assert (mesh.octree ().numDegeneratedFaces () == 0);
  }
};

DELEGATE_BIG3 (ActionOnPostProcessedWMesh)

DELEGATE2 (WingedFace&, ActionOnPostProcessedWMesh, realignFace, WingedMesh&, WingedFace&)
DELEGATE1 (void       , ActionOnPostProcessedWMesh, realignAllFaces, WingedMesh&)
DELEGATE1 (void       , ActionOnPostProcessedWMesh, writeAllNormals, WingedMesh&)
DELEGATE1 (void       , ActionOnPostProcessedWMesh, writeAllIndices, WingedMesh&)
DELEGATE1 (void       , ActionOnPostProcessedWMesh, bufferData, WingedMesh&)
DELEGATE3 (void       , ActionOnPostProcessedWMesh, finalize, WingedMesh&, AffectedFaces&, ActionUnitOn<WingedMesh>&)

void ActionOnPostProcessedWMesh :: runUndo (WingedMesh& mesh) const {
  this->runUndoBeforePostProcessing (mesh);
  this->impl->postProcess           (mesh);
}

void ActionOnPostProcessedWMesh :: runRedo (WingedMesh& mesh) const {
  this->runRedoBeforePostProcessing (mesh);
  this->impl->postProcess           (mesh);
}
