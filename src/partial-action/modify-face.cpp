#include "partial-action/modify-face.hpp"
#include "macro.hpp"
#include "winged-edge.hpp"
#include "winged-face.hpp"
#include "winged-mesh.hpp"
#include "state.hpp"
#include "partial-action/ids.hpp"
#include "adjacent-iterator.hpp"
#include "action/unit.hpp"
#include "partial-action/modify-vertex.hpp"

enum class Operation { Edge, Write };

struct PAModifyFace :: Impl {
  Operation  operation;
  PAIds      operands;
  ActionUnit actions;

  void edge (WingedMesh& mesh, WingedFace& face, WingedEdge* e) {
    this->operation = Operation::Edge;
    this->operands.setIds ({mesh.id (), face.id ()});
    this->operands.setEdge (2, face.edge ());
    face.edge (e);
  }

  void write (WingedMesh& mesh, WingedFace& face) {
    assert (face.isTriangle ());
    this->actions.reset ();
    this->operation = Operation::Write;

    // write indices
    unsigned int indexNumber = face.firstIndexNumber ();
    for (ADJACENT_VERTEX_ITERATOR (it,face)) {
      this->actions.add <PAModifyVertex> ()->writeIndex (mesh, it.element (), indexNumber);
      indexNumber++;
    }
    // write normals
    for (ADJACENT_VERTEX_ITERATOR (it,face)) {
      this->actions.add <PAModifyVertex> ()->writeNormal (mesh, it.element ());
    }
  }

  void toggle () { 
    WingedMesh& mesh =  this->operands.getMesh (0);
    WingedFace& face = *this->operands.getFace (mesh,1);

    switch (this->operation) {
      case Operation::Edge: {
        WingedEdge* e = face.edge ();
        face.edge (this->operands.getEdge (mesh,2)); 
        this->operands.setEdge (2,e);
        break;
      }
      default: assert (false);
    }
  }

  void undo () { 
    switch (this->operation) {
      case Operation::Write: {
        this->actions.undo ();
        break;
      }
      default: {
        this->toggle ();
        break;
      }
    }
  }

  void redo () { 
    switch (this->operation) {
      case Operation::Write: {
        this->actions.redo ();
        break;
      }
      default: {
        this->toggle ();
        break;
      }
    }
  }
};

DELEGATE_CONSTRUCTOR (PAModifyFace)
DELEGATE_DESTRUCTOR  (PAModifyFace)

DELEGATE3 (void,PAModifyFace,edge        ,WingedMesh&,WingedFace&,WingedEdge*)
DELEGATE2 (void,PAModifyFace,write       ,WingedMesh&,WingedFace&)
DELEGATE  (void,PAModifyFace,undo)
DELEGATE  (void,PAModifyFace,redo)
