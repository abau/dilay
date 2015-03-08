#include "action/unit/on-winged-mesh.hpp"

ActionUnitOnWMesh :: ActionUnitOnWMesh () 
  : actions () 
{}

ActionUnitOnWMesh :: ActionUnitOnWMesh (ActionUnitOnWMesh&& other) 
  : actions (std::move (other.actions)) 
{}

bool ActionUnitOnWMesh :: isEmpty () const { 
  return this->actions.empty (); 
}

void ActionUnitOnWMesh :: runUndo (WingedMesh& m) const {
  for (auto it = this->actions.rbegin (); it != this->actions.rend (); ++it) {
    (*it)->undo (m);
  }
}

void ActionUnitOnWMesh :: runRedo (WingedMesh& m) const {
  for (auto it = this->actions.begin (); it != this->actions.end (); ++it) {
    (*it)->redo (m);
  }
}
