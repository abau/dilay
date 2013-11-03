#ifndef DILAY_ACTION_CARVE
#define DILAY_ACTION_CARVE

#include <list>
#include "action.hpp"

class Id;
class WingedMesh;

class ActionCarve : public Action {
  public: 
          ActionCarve            ();
          ActionCarve            (const ActionCarve&) = delete;
    const ActionCarve& operator= (const ActionCarve&) = delete;
         ~ActionCarve            ();

    void run  (WingedMesh&, const glm::vec3&, float);
    void undo ();
    void redo ();

  private:
    class Impl;
    Impl* impl;
};

#endif
