#include <unordered_set>
#include "action/on-post-processed-winged-mesh.hpp"
#include "id.hpp"
#include "winged-face.hpp"
#include "winged-edge.hpp"
#include "winged-mesh.hpp"
#include "triangle.hpp"
#include "adjacent-iterator.hpp"

struct ActionOnPostProcessedWMesh :: Impl {
  std::unordered_set <Id> ids;

  WingedFace& realignFace (WingedMesh& mesh, const WingedFace& face) {
    this->ids.insert (face.id ());
    return this->runRealignFace (mesh, face);
  }

  void realignAllFaces (WingedMesh& mesh) {
    for (const Id& id : this->ids) {
      WingedFace* face = mesh.face (id);
      if (face) {
        this->runRealignFace (mesh, *face);
      }
    }
  }

  WingedFace& runRealignFace (WingedMesh& mesh, const WingedFace& face) {
    Triangle faceTriangle (face.triangle (mesh));
    std::list <WingedEdge*> adjacents = face.adjacentEdgeIterator ().collect ();

    for (WingedEdge* e : adjacents) {
      e->face (face,nullptr);
    }

    bool        sameNode = false;
    WingedFace& newFace  = mesh.realignFace (face, faceTriangle, &sameNode);

    for (WingedEdge* e : adjacents) {
      if (e->leftFace () == nullptr)
        e->leftFace (&newFace);
      else if (e->rightFace () == nullptr)
        e->rightFace (&newFace);
      else
        assert (false);
    }
    if (sameNode) {
    //  this->ids.erase (newFace.id ());
    }
    return newFace;
  }
};

DELEGATE_ACTION_BIG6 (ActionOnPostProcessedWMesh)

DELEGATE2 (WingedFace&, ActionOnPostProcessedWMesh, realignFace, WingedMesh&, const WingedFace&)

void ActionOnPostProcessedWMesh :: runUndo (WingedMesh& mesh) {
  this->runUndoBeforePostProcessing (mesh);
  this->impl->realignAllFaces       (mesh);
}

void ActionOnPostProcessedWMesh :: runRedo (WingedMesh& mesh) {
  this->runRedoBeforePostProcessing (mesh);
  this->impl->realignAllFaces       (mesh);
}
