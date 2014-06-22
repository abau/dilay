#include <glm/glm.hpp>
#include "action/modify-sphere-mesh.hpp"
#include "sphere/mesh.hpp"
#include "action/ids.hpp"

enum class Operation { 
  NewNode, ModifyNode, DeleteNode
};

struct OperandData {
  glm::vec3 position;
  float     radius;
};

struct ActionModifySMesh :: Impl {
  Operation   operation;
  ActionIds   operandIds;
  OperandData operandData;

  void newNode (SphereMesh& mesh, SphereMeshNode* parent, const glm::vec3& pos, float r) {
    this->operation      = Operation::ModifyNode;
    SphereMeshNode& node = mesh.addNode (parent, pos, r);
    this->operandData    = {pos, r};
    this->operandIds.setNode (0, &node);
    this->operandIds.setNode (1,  parent);
  }

  void modifyNode (SphereMeshNode& node, const glm::vec3& pos, float r) {
    this->operation = Operation::ModifyNode;
    this->operandIds.setNode (0, &node);
    this->operandData = {node.position (), node.radius ()};
    node.position (pos);
    node.radius   (r);
  }

  void deleteNode (SphereMesh& mesh, SphereMeshNode& node) {
    assert (node.numChildren () == 0);
    this->operation   = Operation::DeleteNode;
    this->operandData = {node.position (), node.radius ()};
    this->operandIds.setNode (0, &node);
    this->operandIds.setNode (1,  node.parent ());
    mesh.deleteNode (node);
  }

  void toggle (SphereMesh& mesh) {
    switch (this->operation) {
      case Operation::ModifyNode: {
        SphereMeshNode* node = this->operandIds.getSphereMeshNode (mesh, 0);
        glm::vec3       pos  = node->position ();
        float           r    = node->radius   ();
        node->position (this->operandData.position);
        node->radius   (this->operandData.radius);
        this->operandData.position = pos;
        this->operandData.radius   = r;
        break;
      }
      default: assert (false);
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
                     , this->operandData.position, this->operandData.radius);
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
                     , this->operandData.position, this->operandData.radius);
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

DELEGATE4 (void, ActionModifySMesh, newNode   , SphereMesh&, SphereMeshNode*, const glm::vec3&, float)
DELEGATE3 (void, ActionModifySMesh, modifyNode, SphereMeshNode&, const glm::vec3&, float)
DELEGATE1 (void, ActionModifySMesh, runUndo   , SphereMesh&)
DELEGATE1 (void, ActionModifySMesh, runRedo   , SphereMesh&)
