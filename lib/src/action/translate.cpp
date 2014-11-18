#include <glm/glm.hpp>
#include "action/translate.hpp"
#include "selection-mode.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "winged/mesh.hpp"

struct ActionTranslate::Impl {
  SelectionMode              selection;
  std::vector <unsigned int> indices;
  glm::vec3                  delta;

  void translate (const std::list <WingedMesh*>& meshes, const glm::vec3& t) {
    this->selection = SelectionMode::Freeform;
    this->delta     = t;

    for (WingedMesh* m : meshes) {
      this->indices.emplace_back (m->index ());
      m->translate               (this->delta);
      m->normalize               ();
      m->bufferData              ();
    }
  }

  void toggle (float factor) const {
    glm::vec3 toggledDelta = this->delta * factor;

    switch (this->selection) {
      case SelectionMode::Freeform: {
        for (unsigned int index : this->indices) {
          WingedMesh& mesh = State::scene ().wingedMeshRef (index);
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
