#ifndef DILAY_ACTION_TRANSLATE
#define DILAY_ACTION_TRANSLATE

#include <list>
#include <glm/fwd.hpp>
#include "action.hpp"
#include "macro.hpp"

class SphereMesh;

class ActionTranslate : public Action {
  public: 
    DECLARE_BIG3 (ActionTranslate)

    void translate (const std::list <SphereMesh*>&, const glm::vec3&);

  private:
    void runUndo ();
    void runRedo ();

    class Impl;
    Impl* impl;
};

#endif
