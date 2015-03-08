#include "action/on-winged-mesh.hpp"
#include "action/transformer.hpp"
#include "scene.hpp"
#include "winged/mesh.hpp"

ActionTransformer :: ActionTransformer (Scene& s, WingedMesh& m, ActionOnWMesh&& a) 
  : scene     (s)
  , actionPtr (&a) 
  , index     (m.index ())
{}

ActionTransformer :: ~ActionTransformer () {}

void ActionTransformer :: runUndo () const { 
  this->actionPtr->undo (*this->scene.wingedMesh (this->index));
}

void ActionTransformer :: runRedo () const {
  this->actionPtr->redo (*this->scene.wingedMesh (this->index));
}

