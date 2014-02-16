#ifndef DILAY_ACTION_UNIT_ON_WINGED_MESH
#define DILAY_ACTION_UNIT_ON_WINGED_MESH

#include "action/on-winged-mesh.hpp"
#include "macro.hpp"

class WingedMesh;

class ActionUnitOnWMesh : public ActionOnWMesh {
  public: 
    DECLARE_BIG3 (ActionUnitOnWMesh)

    template <class T>
    T* add () { 
      T* action = new T ();
      this->addAction (action); 
      return action; 
    }

    void         addAction  (ActionOnWMesh*);
    void         reset      ();
    unsigned int numActions () const;

  private:
    void runUndo (WingedMesh&);
    void runRedo (WingedMesh&);

    class Impl;
    Impl* impl;
};
#endif
