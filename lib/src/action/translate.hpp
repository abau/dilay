#ifndef DILAY_ACTION_TRANSLATE
#define DILAY_ACTION_TRANSLATE

#include <glm/fwd.hpp>
#include <list>
#include "action.hpp"
#include "macro.hpp"

class WingedMesh;

class ActionTranslate : public Action {
  public: 
    DECLARE_BIG3 (ActionTranslate)

    void translate (const std::list <WingedMesh*>&, const glm::vec3&);

  private:
    void runUndo () const;
    void runRedo () const;

    IMPLEMENTATION
};

#endif
