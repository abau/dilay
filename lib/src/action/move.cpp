#include <glm/glm.hpp>
#include "action/move.hpp"
#include "action/ids.hpp"
#include "selection-mode.hpp"
#include "sphere/mesh.hpp"
#include "id.hpp"

struct ActionMove::Impl {
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

  void position (SphereMesh& mesh, const glm::vec3& p) {
    this->selection = SelectionMode::Sphere;
    this->delta     = p - mesh.position ();
    this->ids.setId (0, mesh.id ());
    mesh.translate  (this->delta);
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
      default:
        assert (false);
    }
  }

  void runUndo () { this->toggle (); }
  void runRedo () { this->toggle (); }
};

DELEGATE_BIG3 (ActionMove)
DELEGATE2 (void, ActionMove, translate, const std::list <SphereMesh*>&, const glm::vec3&)
DELEGATE2 (void, ActionMove, position , SphereMesh&, const glm::vec3&)
DELEGATE  (void, ActionMove, runUndo)
DELEGATE  (void, ActionMove, runRedo)
