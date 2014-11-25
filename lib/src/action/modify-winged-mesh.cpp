#include <glm/glm.hpp>
#include "action/modify-winged-mesh.hpp"
#include "action/data.hpp"
#include "winged/mesh.hpp"

namespace {
  enum class Operation { 
    Translate, Scale
  };

  typedef ActionData <glm::vec3, glm::mat4x4> Data;
};

struct ActionModifyWMesh :: Impl {
  Operation operation;
  Data      data;

  void translate (WingedMesh& mesh, const glm::vec3& delta) {
    this->operation = Operation::Translate;
    this->data.values (-delta, delta);
    mesh.translate    (delta);
    mesh.normalize    ();
    mesh.bufferData   ();
  }

  void position (WingedMesh& mesh, const glm::vec3& pos) {
    this->translate (mesh, pos - mesh.position ());
  }

  void scale (WingedMesh& mesh, const glm::vec3& factor) {
    this->operation = Operation::Scale;
    this->data.values (1.0f/factor, factor);
    mesh.scale        (factor);
    mesh.normalize    ();
    mesh.bufferData   ();
  }

  void toggle (WingedMesh& mesh, ActionDataType type) const {
    switch (this->operation) {
      case Operation::Translate: {
        mesh.translate (this->data.value <glm::vec3> (type));
        break;
      }
      case Operation::Scale: {
        mesh.scale (this->data.value <glm::vec3> (type));
        break;
      }
    }
    mesh.normalize  ();
    mesh.bufferData ();
  }

  void runUndo (WingedMesh& mesh) { this->toggle (mesh, ActionDataType::Old); }
  void runRedo (WingedMesh& mesh) { this->toggle (mesh, ActionDataType::New); }
};

DELEGATE_BIG3 (ActionModifyWMesh)

DELEGATE2       (void, ActionModifyWMesh, translate, WingedMesh&, const glm::vec3&)
DELEGATE2       (void, ActionModifyWMesh, position, WingedMesh&, const glm::vec3&)
DELEGATE2       (void, ActionModifyWMesh, scale, WingedMesh&, const glm::vec3&)
DELEGATE1_CONST (void, ActionModifyWMesh, runUndo , WingedMesh&)
DELEGATE1_CONST (void, ActionModifyWMesh, runRedo , WingedMesh&)
