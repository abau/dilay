#ifndef DILAY_ACTION_UNIT_ON_WINGED_MESH
#define DILAY_ACTION_UNIT_ON_WINGED_MESH

#include <list>
#include <memory>
#include "action/on-winged-mesh.hpp"

class ActionUnitOnWMesh : public ActionOnWMesh {
  public: 
          ActionUnitOnWMesh            ();
          ActionUnitOnWMesh            (ActionUnitOnWMesh&& other);
          ActionUnitOnWMesh            (const ActionUnitOnWMesh&) = delete;
    const ActionUnitOnWMesh& operator= (const ActionUnitOnWMesh&) = delete;
    const ActionUnitOnWMesh& operator= (ActionUnitOnWMesh&&)      = delete;
         ~ActionUnitOnWMesh            () {}

    template <typename A>
    A& add () {
      A& action = *new A ();
      this->actions.push_back (ActionPtr (&action));
      return action;
    }

    bool isEmpty () const;

  private:
    void runUndo (WingedMesh&) const;
    void runRedo (WingedMesh&) const;

    typedef std::unique_ptr <ActionOnWMesh> ActionPtr;
    std::list <ActionPtr> actions;
};
#endif
