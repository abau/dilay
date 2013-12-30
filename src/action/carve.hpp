#ifndef DILAY_ACTION_CARVE
#define DILAY_ACTION_CARVE

#include <list>
#include "action/on-winged-mesh.hpp"
#include "macro.hpp"

class Id;
class WingedMesh;

class ActionCarve : public ActionOnWMesh {
  public: 
    DECLARE_ACTION_BIG6 (ActionCarve)

    void run  (WingedMesh&, const glm::vec3&, float);
    void undo (WingedMesh&);
    void redo (WingedMesh&);

  private:
    class Impl;
    Impl* impl;
};

#endif
