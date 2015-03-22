#ifndef DILAY_HISTORY
#define DILAY_HISTORY

#include "configurable.hpp"
#include "macro.hpp"

class Scene;
class State;

class History : public Configurable {
  public: 
    DECLARE_BIG3 (History, const Config&)

    void snapshot     (const Scene&);
    void dropSnapshot ();
    void undo         (State&);
    void redo         (State&);

  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif
