#include <memory>
#include <glm/glm.hpp>
#include "partial-action/modify-vertex.hpp"
#include "macro.hpp"
#include "winged-edge.hpp"
#include "winged-vertex.hpp"
#include "winged-mesh.hpp"
#include "state.hpp"
#include "partial-action/ids.hpp"

enum class Operation { Edge, WriteIndex, WriteNormal, Move };

struct PAModifyVertex :: Impl {
  Operation                   operation;
  PAIds                       operands;
  std::unique_ptr <glm::vec3> vec3;

  void edge (WingedMesh& mesh, WingedVertex& vertex, WingedEdge* e) {
    this->operation = Operation::Edge;
    this->operands.setMesh   (0, &mesh);
    this->operands.setVertex (0, &vertex);
    this->operands.setEdge   (1, vertex.edge ());
    vertex.edge              (e);
  }

  void writeIndex (WingedMesh& mesh, WingedVertex& vertex, unsigned int indexNumber) {
    this->operation = Operation::WriteIndex;
    this->operands.setMesh   (0, &mesh);
    this->operands.setVertex (0, &vertex);
    this->operands.setIndex  (1, indexNumber);
    this->operands.setIndex  (2, mesh.index (indexNumber));
    vertex.writeIndex        (mesh, indexNumber);
  }

  void writeNormal (WingedMesh& mesh, WingedVertex& vertex) {
    this->operation = Operation::WriteNormal;
    this->operands.setMesh   (0, &mesh);
    this->operands.setVertex (0, &vertex);
    this->vec3.reset         (new glm::vec3 (mesh.normal (vertex.index ())));
    vertex.writeNormal       (mesh);
  }

  void move (WingedMesh& mesh, WingedVertex& vertex, const glm::vec3& pos) {
    this->operation = Operation::Move;
    this->operands.setMesh   (0, &mesh);
    this->operands.setVertex (0, &vertex);
    this->vec3.reset         (new glm::vec3 (vertex.vertex (mesh)));
    mesh.setVertex           (vertex.index (), pos);
  }

  void toggle () { 
    WingedMesh&   mesh   =  this->operands.getMesh (0);
    WingedVertex& vertex = *this->operands.getVertex (mesh,0);

    switch (this->operation) {
      case Operation::Edge: {
        WingedEdge* e = vertex.edge ();
        vertex.edge (this->operands.getEdge (mesh, 1)); 
        this->operands.setEdge (1,e);
        break;
      }
      case Operation::WriteIndex: {
        unsigned int indexNumber = *this->operands.getIndex (1);
        unsigned int index       = mesh.index (indexNumber);
        vertex.writeIndex (mesh, indexNumber);
        this->operands.setIndex (2,index);
        break;
      }
      case Operation::WriteNormal: {
        glm::vec3 normal = mesh.normal (vertex.index ());
        mesh.setNormal   (vertex.index (), *this->vec3);
        this->vec3.reset (new glm::vec3 (normal));
        break;
      }
      case Operation::Move: {
        glm::vec3 pos = mesh.vertex (vertex.index ());
        mesh.setVertex   (vertex.index (), *this->vec3);
        this->vec3.reset (new glm::vec3 (pos));
        break;
      }
      default: assert (false);
    }
  }

  void undo () { this->toggle (); }
  void redo () { this->toggle (); }
};

DELEGATE_CONSTRUCTOR (PAModifyVertex)
DELEGATE_DESTRUCTOR  (PAModifyVertex)

DELEGATE3 (void,PAModifyVertex,edge       ,WingedMesh&,WingedVertex&,WingedEdge*)
DELEGATE3 (void,PAModifyVertex,writeIndex ,WingedMesh&,WingedVertex&,unsigned int)
DELEGATE2 (void,PAModifyVertex,writeNormal,WingedMesh&,WingedVertex&)
DELEGATE3 (void,PAModifyVertex,move,WingedMesh&,WingedVertex&,const glm::vec3&)
DELEGATE  (void,PAModifyVertex,undo)
DELEGATE  (void,PAModifyVertex,redo)
