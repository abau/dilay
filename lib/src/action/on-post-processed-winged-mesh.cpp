#include <unordered_set>
#include "action/on-post-processed-winged-mesh.hpp"
#include "bitset.hpp"
#include "id.hpp"
#include "octree.hpp"
#include "primitive/triangle.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"

struct ActionOnPostProcessedWMesh :: Impl {
  std::unordered_set <Id> realignIds;
  Bitset <char>           flags;

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

  WingedFace& realignFace (WingedMesh& mesh, WingedFace&& face) {
    bool        sameNode  = false;
    WingedFace& realigned = mesh.realignFace ( std::move (face)
                                             , face.triangle (mesh)
                                             , &sameNode );
    if (sameNode == false) {
      this->realignIds.insert (realigned.id ());
    }
    return realigned;
  }

  void realignStoredFaces (WingedMesh& mesh) {
    for (const Id& id : this->realignIds) {
      WingedFace* face = mesh.face (id);
      if (face) {
        mesh.realignFace (std::move (*face), face->triangle (mesh), nullptr);
      }
    }
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

  void postProcess (WingedMesh& mesh) {
    if (this->doRealignAllFaces ()) {
      this->realignAllFaces (mesh);
    }
    else {
      this->realignStoredFaces (mesh);
    }
    if (this->doWriteAllNormals ()) {
      this->writeAllNormals (mesh);
    }
    if (this->doWriteAllIndices ()) {
      this->writeAllIndices (mesh);
    }
    if (this->doBufferData ()) {
      this->bufferData (mesh);
    }
  }
};

DELEGATE_BIG3 (ActionOnPostProcessedWMesh)

DELEGATE2 (WingedFace&, ActionOnPostProcessedWMesh, realignFace, WingedMesh&, WingedFace&&)
DELEGATE1 (void       , ActionOnPostProcessedWMesh, realignAllFaces, WingedMesh&)
DELEGATE1 (void       , ActionOnPostProcessedWMesh, writeAllNormals, WingedMesh&)
DELEGATE1 (void       , ActionOnPostProcessedWMesh, writeAllIndices, WingedMesh&)
DELEGATE1 (void       , ActionOnPostProcessedWMesh, bufferData, WingedMesh&)

void ActionOnPostProcessedWMesh :: runUndo (WingedMesh& mesh) {
  this->runUndoBeforePostProcessing (mesh);
  this->impl->postProcess           (mesh);
}

void ActionOnPostProcessedWMesh :: runRedo (WingedMesh& mesh) {
  this->runRedoBeforePostProcessing (mesh);
  this->impl->postProcess           (mesh);
}
