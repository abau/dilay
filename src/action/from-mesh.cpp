#include "action/from-mesh.hpp"
#include "winged-mesh.hpp"
#include "mesh.hpp"

enum class Operation { Cube, Sphere, Icosphere };

struct ActionFromMesh :: Impl {
  Operation     operation;
  unsigned int  arg1;
  unsigned int  arg2;

  void cube (WingedMesh& mesh) {
    assert (mesh.isEmpty ());
    this->operation = Operation::Cube;
  }

  void sphere (WingedMesh& mesh, unsigned int rings, unsigned int sectors) {
    assert (mesh.isEmpty ());
    this->operation = Operation::Sphere;
    this->arg1      = rings;
    this->arg2      = sectors;
  }

  void icosphere (WingedMesh& mesh, unsigned int numSubdivisons) {
    assert (mesh.isEmpty ());
    this->operation = Operation::Icosphere;
    this->arg1      = numSubdivisons;
  }

  void undo (WingedMesh& mesh) { 
  }

  void redo (WingedMesh& mesh) { 
  }
};

DELEGATE_ACTION_BIG6 (ActionFromMesh)
DELEGATE1            (void, ActionFromMesh, cube, WingedMesh&);
DELEGATE3            (void, ActionFromMesh, sphere, WingedMesh&, unsigned int, unsigned int);
DELEGATE2            (void, ActionFromMesh, icosphere, WingedMesh&, unsigned int);
DELEGATE1            (void, ActionFromMesh, undo, WingedMesh&)
DELEGATE1            (void, ActionFromMesh, redo, WingedMesh&)
