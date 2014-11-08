#include <glm/glm.hpp>
#include "action/modify-winged-mesh.hpp"
#include "variant.hpp"
#include "winged/mesh.hpp"

namespace {
  enum class Operation { 
    Position, RotationMatrix, Scaling
  };

  typedef Variant <glm::vec3, glm::mat4x4> OperandData;
};

struct ActionModifyWMesh :: Impl {
  Operation   operation;
  OperandData operandData;

  void position (WingedMesh& mesh, const glm::vec3& pos) {
    this->operation = Operation::Position;
    this->operandData.set <glm::vec3> (mesh.position ());
    mesh.position (pos);
  }

  void rotationMatrix (WingedMesh& mesh, const glm::mat4x4& mat) {
    this->operation = Operation::RotationMatrix;
    this->operandData.set <glm::mat4x4> (mesh.rotationMatrix ());
    mesh.rotationMatrix (mat);
  }

  void scaling (WingedMesh& mesh, const glm::vec3& scal) {
    this->operation = Operation::Scaling;
    this->operandData.set <glm::vec3> (mesh.scaling ());
    mesh.scaling (scal);
  }

  void toggle (WingedMesh& mesh) {
    switch (this->operation) {
      case Operation::Position: {
        glm::vec3 pos = mesh.position ();
        mesh.position (this->operandData.get <glm::vec3> ());
        this->operandData.set <glm::vec3> (pos);
        break;
      }
      case Operation::RotationMatrix: {
        glm::mat4x4 rot = mesh.rotationMatrix ();
        mesh.rotationMatrix (this->operandData.get <glm::mat4x4> ());
        this->operandData.set <glm::mat4x4> (rot);
        break;
      }
      case Operation::Scaling: {
        glm::vec3 scal = mesh.scaling ();
        mesh.scaling (this->operandData.get <glm::vec3> ());
        this->operandData.set <glm::vec3> (scal);
        break;
      }
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
DELEGATE2 (void, ActionModifyWMesh, rotationMatrix, WingedMesh&, const glm::mat4x4&)
DELEGATE2 (void, ActionModifyWMesh, scaling, WingedMesh&, const glm::vec3&)
DELEGATE1 (void, ActionModifyWMesh, runUndo , WingedMesh&)
DELEGATE1 (void, ActionModifyWMesh, runRedo , WingedMesh&)
