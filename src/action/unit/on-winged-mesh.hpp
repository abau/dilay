#ifndef DILAY_ACTION_UNIT_ON_WINGED_MESH
#define DILAY_ACTION_UNIT_ON_WINGED_MESH

#include "action/on-winged-mesh.hpp"
#include "action/sub-action-kind.hpp"
#include "macro.hpp"

class WingedMesh;

class ActionUnitOnWMesh : public ActionOnWMesh {
  public: 
    DECLARE_ACTION_BIG6 (ActionUnitOnWMesh)

    template <class T>
    T* add (SubActionKind kind = SubActionKind::Normal) { 
      T* action = new T ();
      this->addAction (action, kind); 
      return action; 
    }

    void addAction (ActionOnWMesh*, SubActionKind);
    void reset     ();
    void undo      (WingedMesh&);
    void redo      (WingedMesh&);
  private:
    class Impl;
    Impl* impl;
};
#endif
