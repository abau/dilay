#include "action/data.hpp"
#include "action/unit/on-winged-mesh.hpp"
#include "action/util.hpp"
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
  Operation                         operation;
  ActionUnitOnWMesh                 actions;
  ActionData <Maybe <unsigned int>> data;

  void edge (WingedFace& face, WingedEdge* e) {
    this->operation = Operation::Edge;

    this->data.index  (face);
    this->data.values (ActionUtil::maybeIndex (face.edge ()), ActionUtil::maybeIndex (e));
    face.edge         (e);
  }

  void reset (WingedFace& face) {
    this->edge (face, nullptr);
  }

  void writeIndices (WingedMesh& mesh, WingedFace& face) {
    assert (face.isTriangle ());
    this->operation = Operation::WriteIndices;

    this->actions.add <PAModifyWVertex> ()
                 .writeIndex (mesh, face.vertexRef (0), face.vertexIndex (0));
    this->actions.add <PAModifyWVertex> ()
                 .writeIndex (mesh, face.vertexRef (1), face.vertexIndex (1));
    this->actions.add <PAModifyWVertex> ()
                 .writeIndex (mesh, face.vertexRef (2), face.vertexIndex (2));
  }

  void runUndo (WingedMesh& mesh) const { 
    switch (this->operation) {
      case Operation::Edge: {
        const Maybe<unsigned int> m = this->data.value <Maybe <unsigned int>> (ActionDataType::Old);

        mesh.faceRef (this->data.index ()).edge (ActionUtil::wingedEdge (mesh, m));
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
        const Maybe<unsigned int> m = this->data.value <Maybe <unsigned int>> (ActionDataType::New);

        mesh.faceRef (this->data.index ()).edge (ActionUtil::wingedEdge (mesh, m));
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
