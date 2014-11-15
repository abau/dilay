#include "action/data.hpp"
#include "action/identifier.hpp"
#include "action/unit/on.hpp"
#include "adjacent-iterator.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"

namespace {
  enum class Operation { Edge, WriteIndices };
};

struct PAModifyWFace :: Impl {
  Operation                     operation;
  ActionUnitOn <WingedMesh>     actions;
  ActionData <ActionIdentifier> data;

  void edge (WingedFace& face, WingedEdge* e) {
    this->operation = Operation::Edge;

    this->data.identifier       (face);
    this->data.valueIdentifiers (face.edge (), e);
    face.edge                   (e);
  }

  void reset (WingedFace& face) {
    this->edge (face, nullptr);
  }

  void writeIndices (WingedMesh& mesh, WingedFace& face) {
    assert (face.isTriangle ());
    this->operation = Operation::WriteIndices;

    unsigned int index = face.index ();
    for (WingedVertex& a : face.adjacentVertices ()) {
      this->actions.add <PAModifyWVertex> ().writeIndex (mesh, a, index);
      index++;
    }
  }

  void runUndo (WingedMesh& mesh) const { 
    switch (this->operation) {
      case Operation::Edge: {
        this->data.identifier ().getFaceRef (mesh)
                  .edge (this->data.valueIdentifier (ActionDataType::Old).getEdge (mesh));
        break;
      }
      case Operation::WriteIndices: {
        this->actions.undo (mesh);
        break;
      }
    }
  }

  void runRedo (WingedMesh& mesh) const { 
    switch (this->operation) {
      case Operation::Edge: {
        this->data.identifier ().getFaceRef (mesh)
                  .edge (this->data.valueIdentifier (ActionDataType::New).getEdge (mesh));
        break;
      }
      case Operation::WriteIndices: {
        this->actions.redo (mesh);
        break;
      }
    }
  }
};

DELEGATE_BIG3 (PAModifyWFace)

DELEGATE2       (void, PAModifyWFace, edge , WingedFace&, WingedEdge*)
DELEGATE1       (void, PAModifyWFace, reset , WingedFace&)
DELEGATE2       (void, PAModifyWFace, writeIndices, WingedMesh&, WingedFace&)
DELEGATE1_CONST (void, PAModifyWFace, runUndo , WingedMesh&)
DELEGATE1_CONST (void, PAModifyWFace, runRedo , WingedMesh&)
