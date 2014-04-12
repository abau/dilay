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
  std::unordered_set <Id> ids;
  Bitset <char>           flags;

  bool writeMesh () const { return this->flags.get (0); }
  void writeMesh (bool v) {        this->flags.set (0,v); }

  bool bufferMesh () const { return this->flags.get (1); }
  void bufferMesh (bool v) {        this->flags.set (1,v); }

  WingedFace& realignFace (WingedMesh& mesh, const WingedFace& face) {
    bool        sameNode  = false;
    WingedFace& realigned = this->runRealignFace (mesh, face, &sameNode);

    if (sameNode == false) {
      this->ids.insert (face.id ());
    }
    return realigned;
  }

  void realignAllFaces (WingedMesh& mesh) {
    for (const Id& id : this->ids) {
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
    this->realignAllFaces (mesh);
    if (this->writeMesh ())
      mesh.write ();
    if (this->bufferMesh ())
      mesh.bufferData ();
  }
};

DELEGATE_BIG3 (ActionOnPostProcessedWMesh)

DELEGATE2 (WingedFace&, ActionOnPostProcessedWMesh, realignFace, WingedMesh&, const WingedFace&)
DELEGATE1 (void       , ActionOnPostProcessedWMesh, writeMesh, bool)
DELEGATE1 (void       , ActionOnPostProcessedWMesh, bufferMesh, bool)

void ActionOnPostProcessedWMesh :: runUndo (WingedMesh& mesh) {
  this->runUndoBeforePostProcessing (mesh);
  this->impl->postProcess           (mesh);
}

void ActionOnPostProcessedWMesh :: runRedo (WingedMesh& mesh) {
  this->runRedoBeforePostProcessing (mesh);
  this->impl->postProcess           (mesh);
}
