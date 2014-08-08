#include <unordered_set>
#include "action/on-post-processed-winged-mesh.hpp"
#include "id.hpp"
#include "winged/face.hpp"
#include "winged/edge.hpp"
#include "winged/mesh.hpp"
#include "primitive/triangle.hpp"
#include "adjacent-iterator.hpp"
#include "bitset.hpp"

struct ActionOnPostProcessedWMesh :: Impl {
  std::unordered_set <Id>           realignIds;
  Bitset <char>                     flags;

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
    WingedFace& realigned = this->runRealignFace (mesh, face, &sameNode);

    if (sameNode == false) {
      this->realignIds.insert (realigned.id ());
    }
    return realigned;
  }

  void realignStoredFaces (WingedMesh& mesh) {
    for (const Id& id : this->realignIds) {
      WingedFace* face = mesh.face (id);
      if (face) {
        this->runRealignFace (mesh, *face);
      }
    }
  }

  WingedFace& runRealignFace (WingedMesh& mesh, const WingedFace& face, bool* sameNode = nullptr) {
    PrimTriangle faceTriangle (face.triangle (mesh));
    std::list <WingedEdge*> adjacents = face.adjacentEdgeIterator ().collect ();

    for (WingedEdge* e : adjacents) {
      e->face (face,nullptr);
    }

    WingedFace& newFace = mesh.realignFace (face, faceTriangle, sameNode);

    for (WingedEdge* e : adjacents) {
      if (e->leftFace () == nullptr)
        e->leftFace (&newFace);
      else if (e->rightFace () == nullptr)
        e->rightFace (&newFace);
      else
        assert (false);
    }
    return newFace;
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
