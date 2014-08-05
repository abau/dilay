#include "partial-action/modify-winged-face.hpp"
#include "winged/edge.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "state.hpp"
#include "action/ids.hpp"
#include "adjacent-iterator.hpp"
#include "action/unit/on.hpp"
#include "partial-action/modify-winged-vertex.hpp"

namespace {
  enum class Operation { Edge, Write };
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

  void write (WingedMesh& mesh, WingedFace& face) {
    assert (face.isTriangle ());
    assert (this->actions.isEmpty ());
    this->operation = Operation::Write;

    // write indices
    unsigned int indexNumber = face.firstIndexNumber ();
    for (ADJACENT_VERTEX_ITERATOR (it,face)) {
      this->actions.add <PAModifyWVertex> ().writeIndex (mesh, it.element (), indexNumber);
      indexNumber++;
    }
    // write normals
    for (ADJACENT_VERTEX_ITERATOR (it,face)) {
      this->actions.add <PAModifyWVertex> ().writeNormal (mesh, it.element ());
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
      default: assert (false);
    }
  }

  void runUndo (WingedMesh& mesh) { 
    switch (this->operation) {
      case Operation::Write: {
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
      case Operation::Write: {
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
DELEGATE2 (void,PAModifyWFace,write,WingedMesh&,WingedFace&)
DELEGATE1 (void,PAModifyWFace,runUndo ,WingedMesh&)
DELEGATE1 (void,PAModifyWFace,runRedo ,WingedMesh&)
