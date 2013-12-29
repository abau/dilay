#ifndef DILAY_ACTION_UNIT_WINGED_MESH
#define DILAY_ACTION_UNIT_WINGED_MESH

#include "action/on-winged-mesh.hpp"
#include "macro.hpp"

class WingedMesh;

class ActionUnitWMesh : public ActionOnWMesh {
  public: 
    DECLARE_ACTION_BIG5 (ActionUnitWMesh)

    template <class T>
    T* add () { 
      T* action = new T ();
      this->addAction (action); 
      return action; 
    }

    void addAction (ActionOnWMesh*);
    void reset     ();
    void undo      (WingedMesh&);
    void redo      (WingedMesh&);
  private:
    class Impl;
    Impl* impl;
};
#endif
