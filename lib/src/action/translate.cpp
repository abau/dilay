#include <glm/glm.hpp>
#include "action/translate.hpp"
#include "winged/mesh.hpp"

struct ActionTranslate::Impl {
  glm::vec3 delta;

  void translate (WingedMesh& mesh, const glm::vec3& t) {
    this->delta = t;

    mesh.translate  (t);
    mesh.normalize  ();
    mesh.bufferData ();
  }

  void toggle (WingedMesh& mesh, float factor) const {
    mesh.translate  (this->delta * factor);
    mesh.normalize  ();
    mesh.bufferData ();
  }

  void runUndo (WingedMesh& mesh) const { this->toggle (mesh, -1.0f); }
  void runRedo (WingedMesh& mesh) const { this->toggle (mesh,  1.0f); }
};

DELEGATE_BIG3   (ActionTranslate)
DELEGATE2       (void, ActionTranslate, translate, WingedMesh&, const glm::vec3&)
DELEGATE1_CONST (void, ActionTranslate, runUndo, WingedMesh&)
DELEGATE1_CONST (void, ActionTranslate, runRedo, WingedMesh&)
