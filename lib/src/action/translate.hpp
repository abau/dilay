#ifndef DILAY_ACTION_TRANSLATE
#define DILAY_ACTION_TRANSLATE

#include <glm/fwd.hpp>
#include "action/on.hpp"
#include "macro.hpp"

class WingedMesh;

class ActionTranslate : public ActionOn <WingedMesh> {
  public: 
    DECLARE_BIG3 (ActionTranslate)

    void translate (WingedMesh&, const glm::vec3&);

  private:
    void runUndo (WingedMesh&) const;
    void runRedo (WingedMesh&) const;

    IMPLEMENTATION
};

#endif
