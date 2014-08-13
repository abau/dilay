#include <glm/glm.hpp>
#include "action/translate.hpp"
#include "action/ids.hpp"
#include "selection-mode.hpp"
#include "sphere/mesh.hpp"
#include "winged/mesh.hpp"
#include "id.hpp"

struct ActionTranslate::Impl {
  SelectionMode selection;
  ActionIds     ids;
  glm::vec3     delta;

  void translate (const std::list <SphereMesh*>& meshes, const glm::vec3& t) {
    this->selection = SelectionMode::Sphere;
    this->delta     = t;

    unsigned int i = 0;
    for (SphereMesh* m : meshes) {
      this->ids.setId (i, m->id ());
      m->translate    (this->delta);
      i = i + 1;
    }
  }

  void translate (const std::list <WingedMesh*>& meshes, const glm::vec3& t) {
    this->selection = SelectionMode::Freeform;
    this->delta     = t;

    unsigned int i = 0;
    for (WingedMesh* m : meshes) {
      this->ids.setId (i, m->id ());
      m->translate    (this->delta);
      m->normalize    ();
      i = i + 1;
    }
  }

  void toggle () {
    this->delta = -this->delta;

    switch (this->selection) {
      case SelectionMode::Sphere: {
        for (unsigned int i = 0; i < this->ids.numIds (); i++) {
          this->ids.getSphereMesh (i).translate (this->delta);
        }
        break;
      }
      case SelectionMode::Freeform: {
        for (unsigned int i = 0; i < this->ids.numIds (); i++) {
          WingedMesh& mesh = this->ids.getWingedMesh (i);
          mesh.translate (this->delta);
          mesh.normalize ();
        }
        break;
      }
      default:
        assert (false);
    }
  }

  void runUndo () { this->toggle (); }
  void runRedo () { this->toggle (); }
};

DELEGATE_BIG3 (ActionTranslate)
DELEGATE2 (void, ActionTranslate, translate, const std::list <SphereMesh*>&, const glm::vec3&)
DELEGATE2 (void, ActionTranslate, translate, const std::list <WingedMesh*>&, const glm::vec3&)
DELEGATE  (void, ActionTranslate, runUndo)
DELEGATE  (void, ActionTranslate, runRedo)
