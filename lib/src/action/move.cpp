#include <glm/glm.hpp>
#include "action/move.hpp"
#include "action/ids.hpp"
#include "selection-mode.hpp"
#include "sphere/mesh.hpp"
#include "id.hpp"

struct ActionMove::Impl {
  SelectionMode selection;
  ActionIds     ids;
  glm::vec3     prevPosition;

  void position (SphereMesh& mesh, const glm::vec3& p) {
    this->selection = SelectionMode::Sphere;
    this->ids.setId (0, mesh.id ());
    this->prevPosition = mesh.position ();
    mesh.position (p);
  }

  void toggle () {
    switch (this->selection) {
      case SelectionMode::Sphere: {
        SphereMesh& mesh = this->ids.getSphereMesh (0);
        glm::vec3   p    = mesh.position ();
        mesh.position (this->prevPosition);
        this->prevPosition = p;
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
//DELEGATE2 (void, ActionMove, translate, SphereMesh&, const glm::vec3&)
DELEGATE2 (void, ActionMove, position , SphereMesh&, const glm::vec3&)
DELEGATE  (void, ActionMove, runUndo)
DELEGATE  (void, ActionMove, runRedo)
