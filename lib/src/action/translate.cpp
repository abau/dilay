#include <glm/glm.hpp>
#include "action/translate.hpp"
#include "action/identifier.hpp"
#include "selection-mode.hpp"
#include "winged/mesh.hpp"

struct ActionTranslate::Impl {
  SelectionMode                  selection;
  std::vector <ActionIdentifier> ids;
  glm::vec3                      delta;

  void translate (const std::list <WingedMesh*>& meshes, const glm::vec3& t) {
    this->selection = SelectionMode::Freeform;
    this->delta     = t;

    for (WingedMesh* m : meshes) {
      this->ids.emplace_back (m);
      m->translate           (this->delta);
      m->normalize           ();
      m->bufferData          ();
    }
  }

  void toggle (float factor) const {
    glm::vec3 toggledDelta = this->delta * factor;

    switch (this->selection) {
      case SelectionMode::Freeform: {
        for (const ActionIdentifier& id : this->ids) {
          WingedMesh& mesh = id.getWingedMeshRef ();
          mesh.translate  (toggledDelta);
          mesh.normalize  ();
          mesh.bufferData ();
        }
        break;
      }
      default:
        std::abort ();
    }
  }

  void runUndo () const { this->toggle (-1.0f); }
  void runRedo () const { this->toggle ( 1.0f); }
};

DELEGATE_BIG3  (ActionTranslate)
DELEGATE2      (void, ActionTranslate, translate, const std::list <WingedMesh*>&, const glm::vec3&)
DELEGATE_CONST (void, ActionTranslate, runUndo)
DELEGATE_CONST (void, ActionTranslate, runRedo)
