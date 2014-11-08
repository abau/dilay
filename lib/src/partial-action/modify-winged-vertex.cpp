#include <glm/glm.hpp>
#include <memory>
#include "action/ids.hpp"
#include "partial-action/modify-winged-vertex.hpp"
#include "state.hpp"
#include "winged/edge.hpp"
#include "winged/mesh.hpp"
#include "winged/vertex.hpp"

namespace {
  enum class Operation { Edge, WriteIndex, WriteNormal, Move };
};

struct PAModifyWVertex :: Impl {
  Operation                   operation;
  ActionIds                   operands;
  std::unique_ptr <glm::vec3> vec3;

  void edge (WingedVertex& vertex, WingedEdge* e) {
    this->operation = Operation::Edge;
    this->operands.setVertex (0, &vertex);
    this->operands.setEdge   (0, vertex.edge ());
    vertex.edge              (e);
  }

  void writeIndex (WingedMesh& mesh, WingedVertex& vertex, unsigned int index) {
    this->operation = Operation::WriteIndex;
    this->operands.setVertex (0, &vertex);
    this->operands.setIndex  (1, index);
    this->operands.setIndex  (2, mesh.index (index));
    vertex.writeIndex        (mesh, index);
  }

  void writeNormal (WingedMesh& mesh, WingedVertex& vertex, const glm::vec3& normal) {
    this->operation = Operation::WriteNormal;
    this->operands.setVertex (0, &vertex);
    this->vec3.reset         (new glm::vec3 (vertex.savedNormal (mesh)));
    vertex.writeNormal       (mesh, normal);
  }

  void writeInterpolatedNormal (WingedMesh& mesh, WingedVertex& vertex) {
    this->writeNormal (mesh, vertex, vertex.interpolatedNormal (mesh));
  }

  void move (WingedMesh& mesh, WingedVertex& vertex, const glm::vec3& pos) {
    this->moved    (vertex, vertex.vector (mesh));
    mesh.setVertex (vertex.index (), pos);
  }

  void moved (WingedVertex& vertex, const glm::vec3& from) {
    this->operation = Operation::Move;
    this->operands.setVertex (0, &vertex);
    this->vec3.reset         (new glm::vec3 (from));
  }

  void toggle (WingedMesh& mesh) { 
    WingedVertex& vertex = this->operands.getVertexRef (mesh,0);

    switch (this->operation) {
      case Operation::Edge: {
        WingedEdge* e = vertex.edge ();
        vertex.edge (this->operands.getEdge (mesh, 0)); 
        this->operands.setEdge (0,e);
        break;
      }
      case Operation::WriteIndex: {
        unsigned int index       = this->operands.getIndexRef (1);
        unsigned int vertexIndex = mesh.index (index);
        mesh.setIndex (index, this->operands.getIndexRef (2));
        this->operands.setIndex (2,vertexIndex);
        break;
      }
      case Operation::WriteNormal: {
        glm::vec3 normal = mesh.normal (vertex.index ());
        mesh.setNormal   (vertex.index (), *this->vec3);
        this->vec3.reset (new glm::vec3 (normal));
        break;
      }
      case Operation::Move: {
        glm::vec3 pos = mesh.vector (vertex.index ());
        mesh.setVertex   (vertex.index (), *this->vec3);
        this->vec3.reset (new glm::vec3 (pos));
        break;
      }
    }
  }

  void runUndo (WingedMesh& mesh) { this->toggle (mesh); }
  void runRedo (WingedMesh& mesh) { this->toggle (mesh); }
};

DELEGATE_BIG3 (PAModifyWVertex)

DELEGATE2 (void,PAModifyWVertex,edge       ,WingedVertex&,WingedEdge*)
DELEGATE3 (void,PAModifyWVertex,writeIndex ,WingedMesh&,WingedVertex&,unsigned int)
DELEGATE3 (void,PAModifyWVertex,writeNormal,WingedMesh&,WingedVertex&, const glm::vec3&)
DELEGATE2 (void,PAModifyWVertex,writeInterpolatedNormal,WingedMesh&,WingedVertex&)
DELEGATE3 (void,PAModifyWVertex,move,WingedMesh&,WingedVertex&,const glm::vec3&)
DELEGATE2 (void,PAModifyWVertex,moved,WingedVertex&,const glm::vec3&)
DELEGATE1 (void,PAModifyWVertex,runUndo,WingedMesh&)
DELEGATE1 (void,PAModifyWVertex,runRedo,WingedMesh&)
