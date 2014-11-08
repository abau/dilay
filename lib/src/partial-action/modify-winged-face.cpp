#include "action/ids.hpp"
#include "action/unit/on.hpp"
#include "adjacent-iterator.hpp"
#include "partial-action/modify-winged-face.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "state.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"

namespace {
  enum class Operation { Edge, WriteIndices };
};

struct PAModifyWFace :: Impl {
  Operation                 operation;
  ActionIds                 operands;
  ActionUnitOn <WingedMesh> actions;

  void edge (WingedFace& face, WingedEdge* e) {
    this->operation = Operation::Edge;
    this->operands.setFace (0, &face);
    this->operands.setEdge (1,  face.edge ());
    face.edge (e);
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

  void toggle (WingedMesh& mesh) { 
    WingedFace& face = this->operands.getFaceRef (mesh,0);

    switch (this->operation) {
      case Operation::Edge: {
        WingedEdge* e = face.edge ();
        face.edge (this->operands.getEdge (mesh,1)); 
        this->operands.setEdge (1,e);
        break;
      }
      default: std::abort ();
    }
  }

  void runUndo (WingedMesh& mesh) { 
    switch (this->operation) {
      case Operation::WriteIndices: {
        this->actions.undo (mesh);
        break;
      }
      default: {
        this->toggle (mesh);
        break;
      }
    }
  }

  void runRedo (WingedMesh& mesh) { 
    switch (this->operation) {
      case Operation::WriteIndices: {
        this->actions.redo (mesh);
        break;
      }
      default: {
        this->toggle (mesh);
        break;
      }
    }
  }
};

DELEGATE_BIG3 (PAModifyWFace)

DELEGATE2 (void,PAModifyWFace,edge ,WingedFace&,WingedEdge*)
DELEGATE2 (void,PAModifyWFace,writeIndices,WingedMesh&,WingedFace&)
DELEGATE1 (void,PAModifyWFace,runUndo ,WingedMesh&)
DELEGATE1 (void,PAModifyWFace,runRedo ,WingedMesh&)
