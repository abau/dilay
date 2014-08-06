#include <unordered_set>
#include "action/on-post-processed-winged-mesh.hpp"
#include "id.hpp"
#include "winged/face.hpp"
#include "winged/edge.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"
#include "primitive/triangle.hpp"
#include "adjacent-iterator.hpp"
#include "bitset.hpp"

struct ActionOnPostProcessedWMesh :: Impl {
  std::unordered_set <Id>           realignIds;
  std::unordered_set <Id>           writeIndicesIds;
  std::unordered_set <unsigned int> writeNormalIndices;
  Bitset <char>                     flags;

  Impl () {
    this->doBufferData      (false);
    this->doWriteAllIndices (false);
    this->doWriteAllNormals (false);
  }

  bool doBufferData      () const { return this->flags.get (0);   }
  void doBufferData      (bool v) {        this->flags.set (0,v); }
  bool doWriteAllIndices () const { return this->flags.get (1);   }
  void doWriteAllIndices (bool v) {        this->flags.set (1,v); }
  bool doWriteAllNormals () const { return this->flags.get (2);   }
  void doWriteAllNormals (bool v) {        this->flags.set (2,v); }

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

  void writeAllIndices (WingedMesh& mesh) {
    this->doWriteAllIndices (true);
    mesh.writeAllIndices    ();
  }

  void writeIndices (WingedMesh& mesh, WingedFace& face) {
    face.writeIndices (mesh);
    this->writeIndicesIds.insert (face.id ());
  }

  void writeStoredIndices (WingedMesh& mesh) {
    for (const Id& id : this->writeIndicesIds) {
      WingedFace* face = mesh.face (id);
      if (face) {
        face->writeIndices (mesh);
      }
    }
  }

  void writeAllNormals (WingedMesh& mesh) {
    this->doWriteAllNormals (true);
    mesh.writeAllNormals    ();
  }

  void writeNormals (WingedMesh& mesh, WingedFace& face) {
    for (ADJACENT_VERTEX_ITERATOR (it,face)) {
      this->writeNormal (mesh, it.element ());
    }
  }

  void writeNormal (WingedMesh& mesh, WingedVertex& vertex) {
    vertex.writeNormal (mesh);
    this->writeNormalIndices.insert (vertex.index ());
  }

  void writeStoredNormals (WingedMesh& mesh) {
    for (unsigned int index : this->writeNormalIndices) {
      WingedVertex* vertex = mesh.vertex (index);
      if (vertex) {
        vertex->writeNormal (mesh);
      }
    }
  }

  void postProcess (WingedMesh& mesh) {
    this->realignStoredFaces (mesh);

    if (this->doWriteAllIndices ()) {
      mesh.writeAllIndices ();
    }
    else {
      this->writeStoredIndices (mesh);
    }

    if (this->doWriteAllNormals ()) {
      mesh.writeAllNormals ();
    }
    else {
      this->writeStoredNormals (mesh);
    }

    if (this->doBufferData ()) {
      mesh.bufferData ();
    }
  }
};

DELEGATE_BIG3 (ActionOnPostProcessedWMesh)

DELEGATE1      (void       , ActionOnPostProcessedWMesh, bufferData, WingedMesh&)
DELEGATE2      (WingedFace&, ActionOnPostProcessedWMesh, realignFace, WingedMesh&, const WingedFace&)
DELEGATE1      (void       , ActionOnPostProcessedWMesh, writeAllIndices, WingedMesh&)
DELEGATE2      (void       , ActionOnPostProcessedWMesh, writeIndices, WingedMesh&, WingedFace&)
DELEGATE1      (void       , ActionOnPostProcessedWMesh, writeAllNormals, WingedMesh&)
DELEGATE2      (void       , ActionOnPostProcessedWMesh, writeNormals, WingedMesh&, WingedFace&)
DELEGATE2      (void       , ActionOnPostProcessedWMesh, writeNormal, WingedMesh&, WingedVertex&)

void ActionOnPostProcessedWMesh :: runUndo (WingedMesh& mesh) {
  this->runUndoBeforePostProcessing (mesh);
  this->impl->postProcess           (mesh);
}

void ActionOnPostProcessedWMesh :: runRedo (WingedMesh& mesh) {
  this->runRedoBeforePostProcessing (mesh);
  this->impl->postProcess           (mesh);
}
