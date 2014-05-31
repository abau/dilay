#include <glm/glm.hpp>
#include "action/modify-sphere-mesh.hpp"
#include "sphere/mesh.hpp"
#include "action/ids.hpp"
#include "variant.hpp"

enum class Operation { 
  SetPosition
};

struct ActionModifySMesh :: Impl {
  Operation operation;
  ActionIds operandIds;

  Variant <glm::vec3> operandData;

  void setPosition (SphereMeshNode& node, const glm::vec3& pos) {
    this->operation = Operation::SetPosition;
    this->operandIds.setNode (0, &node);
    this->operandData.set <glm::vec3> ({node.position ()});
    node.position (pos);
  }

  void toggle (SphereMesh& mesh) {
    switch (this->operation) {
      case Operation::SetPosition: {
        SphereMeshNode* node = this->operandIds.getSphereMeshNode (mesh, 0);
        glm::vec3       pos  = node->position ();
        node->position (this->operandData.get <glm::vec3> ());
        this->operandData.set <glm::vec3> (pos);
      }
      default: assert (false);
    }
  }

  void runUndo (SphereMesh& mesh) { this->toggle (mesh); }
  void runRedo (SphereMesh& mesh) { this->toggle (mesh); }
};

DELEGATE_BIG3 (ActionModifySMesh)

DELEGATE2 (void, ActionModifySMesh, setPosition, SphereMeshNode&, const glm::vec3&)
DELEGATE1 (void, ActionModifySMesh, runUndo, SphereMesh&)
DELEGATE1 (void, ActionModifySMesh, runRedo, SphereMesh&)
