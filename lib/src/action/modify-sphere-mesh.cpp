#include <glm/glm.hpp>
#include "action/modify-sphere-mesh.hpp"
#include "sphere/mesh.hpp"
#include "action/ids.hpp"
#include "variant.hpp"

namespace {
  enum class Operation { 
    Position, NewNode, DeleteNode
  };

  struct NodeData {
    glm::vec3 position;
    float     radius;
  };

  typedef Variant <NodeData, glm::vec3> OperandData;
};

struct ActionModifySMesh :: Impl {
  Operation   operation;
  ActionIds   operandIds;
  OperandData operandData;

  void position (SphereMesh& mesh, const glm::vec3& pos) {
    this->operation      = Operation::Position;
    this->operandData.set <glm::vec3> (mesh.position ());
    mesh.position (pos);
  }

  void newNode (SphereMesh& mesh, SphereMeshNode* parent, const glm::vec3& pos, float r) {
    this->operation      = Operation::NewNode;
    SphereMeshNode& node = mesh.addNode (parent, pos, r);
    this->operandData.set <NodeData> ({pos, r});
    this->operandIds.setNode (0, &node);
    this->operandIds.setNode (1, parent);
  }

  void deleteNode (SphereMesh& mesh, SphereMeshNode& node) {
    assert (node.numChildren () == 0);
    this->operation   = Operation::DeleteNode;
    this->operandData.set <NodeData> ({node.position (), node.radius ()});
    this->operandIds.setNode (0, &node);
    this->operandIds.setNode (1, node.parent ());
    mesh.deleteNode (node);
  }

  void toggle (SphereMesh& mesh) {
    switch (this->operation) {
      case Operation::Position: {
        glm::vec3 pos = mesh.position ();
        mesh.position (this->operandData.get <glm::vec3> ());
        this->operandData.set <glm::vec3> (pos);
        break;
      }
      default:
        assert (false);
    }
  }

  void runUndo (SphereMesh& mesh) { 
    switch (this->operation) {
      case Operation::NewNode:
        mesh.deleteNode (mesh.node (this->operandIds.getIdRef (0)));
        break;
      case Operation::DeleteNode:
        mesh.addNode ( this->operandIds.getIdRef (0)
                     , this->operandIds.getSphereMeshNode (mesh, 1)
                     , this->operandData.get <NodeData> ().position
                     , this->operandData.get <NodeData> ().radius);
        break;
      default:
        this->toggle (mesh);
    }
  }

  void runRedo (SphereMesh& mesh) { 
    switch (this->operation) {
      case Operation::NewNode:
        mesh.addNode ( this->operandIds.getIdRef (0)
                     , this->operandIds.getSphereMeshNode (mesh, 1)
                     , this->operandData.get <NodeData> ().position
                     , this->operandData.get <NodeData> ().radius);
        break;
      case Operation::DeleteNode:
        mesh.deleteNode (mesh.node (this->operandIds.getIdRef (0)));
        break;
      default:
        this->toggle (mesh);
    }
  }
};

DELEGATE_BIG3 (ActionModifySMesh)

DELEGATE2 (void, ActionModifySMesh, position  , SphereMesh&, const glm::vec3&)
DELEGATE4 (void, ActionModifySMesh, newNode   , SphereMesh&, SphereMeshNode*, const glm::vec3&, float)
DELEGATE2 (void, ActionModifySMesh, deleteNode, SphereMesh&, SphereMeshNode&);
DELEGATE1 (void, ActionModifySMesh, runUndo   , SphereMesh&)
DELEGATE1 (void, ActionModifySMesh, runRedo   , SphereMesh&)
