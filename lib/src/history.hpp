#ifndef DILAY_HISTORY
#define DILAY_HISTORY

#include "action/transformer.hpp"

class Action;
class ActionUnit;
class Scene;
class WingedMesh;

class History {
  public: 
    DECLARE_BIG3 (History)

    template <typename A>
    A& add () { 
      A* action = new A ();
      this->addAction (action); 
      return *action; 
    }

    template <typename A, typename T>
    A& add (Scene& scene, T& t) { 
      A* action = new A ();
      this->addAction (new ActionTransformer <T> (scene, t, action));
      return *action; 
    }

    void addUnit (ActionUnit&&);
    void reset   ();
    void undo    ();
    void redo    ();

  private:
    void addAction (Action*);
    IMPLEMENTATION
};

#endif
