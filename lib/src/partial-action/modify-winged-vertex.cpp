#include <glm/glm.hpp>
#include "action/data.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "winged/edge.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {
  enum class Operation { Edge, WriteIndex, WriteNormal, Move };

  typedef ActionData <ActionIdentifier, unsigned int, glm::vec3> Data;
};

struct PAModifyWVertex :: Impl {
  Operation operation;
  Data      data;

  void edge (WingedVertex& vertex, WingedEdge* e) {
    this->operation = Operation::Edge;

    this->data.identifier       (vertex);
    this->data.valueIdentifiers (vertex.edge (), e);
    vertex.edge                 (e);
  }

  void reset (WingedVertex& vertex) {
    this->edge (vertex, nullptr);
  }

  void writeIndex (WingedMesh& mesh, WingedVertex& vertex, unsigned int index) {
    this->operation = Operation::WriteIndex;

    this->data.identifier (index);
    this->data.values     (mesh.index (index), vertex.index ());
    mesh.setIndex         (index, vertex.index ());
  }

  void writeNormal (WingedMesh& mesh, WingedVertex& vertex, const glm::vec3& normal) {
    this->operation = Operation::WriteNormal;

    this->data.identifier (vertex);
    this->data.values     (vertex.savedNormal (mesh), normal);
    vertex.writeNormal    (mesh, normal);
  }

  void writeInterpolatedNormal (WingedMesh& mesh, WingedVertex& vertex) {
    this->writeNormal (mesh, vertex, vertex.interpolatedNormal (mesh));
  }

  void move (WingedMesh& mesh, WingedVertex& vertex, const glm::vec3& pos) {
    this->operation = Operation::Move;

    this->data.identifier (vertex);
    this->data.values     (vertex.position (mesh), pos);
    vertex.writePosition  (mesh, pos);
  }

  void moved (WingedMesh& mesh, WingedVertex& vertex, const glm::vec3& from) {
    this->operation = Operation::Move;

    this->data.identifier (vertex);
    this->data.values     (from, vertex.position (mesh));
  }

  void toggle (WingedMesh& mesh, ActionDataType type) const { 
    const ActionIdentifier& id = this->data.identifier ();

    switch (this->operation) {
      case Operation::Edge: {
        id.getVertexRef (mesh).edge (this->data.valueIdentifier (type).getEdge (mesh));
        break;
      }
      case Operation::WriteIndex: {
        mesh.setIndex (id.getIndexRef (), this->data.value <unsigned int> (type));
        break;
      }
      case Operation::WriteNormal: {
        id.getVertexRef (mesh).writeNormal (mesh, this->data.value <glm::vec3> (type));
        break;
      }
      case Operation::Move: {
        id.getVertexRef (mesh).writePosition (mesh, this->data.value <glm::vec3> (type));
        break;
      }
    }
  }

  void runUndo (WingedMesh& mesh) const { this->toggle (mesh, ActionDataType::Old); }
  void runRedo (WingedMesh& mesh) const { this->toggle (mesh, ActionDataType::New); }
};

DELEGATE_BIG3 (PAModifyWVertex)

DELEGATE2       (void, PAModifyWVertex, edge       , WingedVertex&, WingedEdge*)
DELEGATE1       (void, PAModifyWVertex, reset      , WingedVertex&)
DELEGATE3       (void, PAModifyWVertex, writeIndex , WingedMesh&, WingedVertex&, unsigned int)
DELEGATE3       (void, PAModifyWVertex, writeNormal, WingedMesh&, WingedVertex&, const glm::vec3&)
DELEGATE2       (void, PAModifyWVertex, writeInterpolatedNormal, WingedMesh&, WingedVertex&)
DELEGATE3       (void, PAModifyWVertex, move, WingedMesh&, WingedVertex&, const glm::vec3&)
DELEGATE3       (void, PAModifyWVertex, moved, WingedMesh&, WingedVertex&, const glm::vec3&)
DELEGATE1_CONST (void, PAModifyWVertex, runUndo,WingedMesh&)
DELEGATE1_CONST (void, PAModifyWVertex, runRedo,WingedMesh&)
