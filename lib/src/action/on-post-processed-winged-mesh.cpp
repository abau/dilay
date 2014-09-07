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
    this->doBufferData (false);
  }

  bool doBufferData () const { return this->flags.get (0);   }
  void doBufferData (bool v) {        this->flags.set (0,v); }

  void bufferData (WingedMesh& mesh) {
    this->doBufferData (true);
    mesh.bufferData ();
  }

  WingedFace& realignFace (WingedMesh& mesh, const WingedFace& face) {
    bool        sameNode  = false;
    WingedFace& realigned = mesh.realignFace (face, face.triangle (mesh), &sameNode);

    if (sameNode == false) {
      this->realignIds.insert (realigned.id ());
    }
    return realigned;
  }

  void realignStoredFaces (WingedMesh& mesh) {
    for (const Id& id : this->realignIds) {
      WingedFace* face = mesh.face (id);
      if (face) {
        mesh.realignFace (*face, face->triangle (mesh), nullptr);
      }
    }
  }

  void postProcess (WingedMesh& mesh) {
    this->realignStoredFaces (mesh);

    if (this->doBufferData ()) {
      mesh.bufferData ();
    }
  }
};

DELEGATE_BIG3 (ActionOnPostProcessedWMesh)

DELEGATE1 (void       , ActionOnPostProcessedWMesh, bufferData, WingedMesh&)
DELEGATE2 (WingedFace&, ActionOnPostProcessedWMesh, realignFace, WingedMesh&, const WingedFace&)

void ActionOnPostProcessedWMesh :: runUndo (WingedMesh& mesh) {
  this->runUndoBeforePostProcessing (mesh);
  this->impl->postProcess           (mesh);
}

void ActionOnPostProcessedWMesh :: runRedo (WingedMesh& mesh) {
  this->runRedoBeforePostProcessing (mesh);
  this->impl->postProcess           (mesh);
}
