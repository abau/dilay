#include <glm/glm.hpp>
#include "action/modify-winged-mesh.hpp"
#include "winged/mesh.hpp"
#include "variant.hpp"

enum class Operation { 
  Position
};

typedef Variant <glm::vec3> OperandData;

struct ActionModifyWMesh :: Impl {
  Operation   operation;
  OperandData operandData;

  void position (WingedMesh& mesh, const glm::vec3& pos) {
    this->operation = Operation::Position;
    this->operandData.set <glm::vec3> (mesh.position ());
    mesh.position (pos);
  }

  void toggle (WingedMesh& mesh) {
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

  void runUndo (WingedMesh& mesh) { 
    this->toggle (mesh);
  }

  void runRedo (WingedMesh& mesh) { 
    this->toggle (mesh);
  }
};

DELEGATE_BIG3 (ActionModifyWMesh)

DELEGATE2 (void, ActionModifyWMesh, position, WingedMesh&, const glm::vec3&)
DELEGATE1 (void, ActionModifyWMesh, runUndo , WingedMesh&)
DELEGATE1 (void, ActionModifyWMesh, runRedo , WingedMesh&)
