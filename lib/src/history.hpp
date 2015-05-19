#ifndef DILAY_HISTORY
#define DILAY_HISTORY

#include "configurable.hpp"
#include "macro.hpp"

class Mesh;
class Octree;
class Scene;

class History : public Configurable {
  public: 
    DECLARE_BIG3 (History, const Config&)

    void snapshot            (const Scene&);
    void dropSnapshot        ();
    void undo                (Scene&);
    void redo                (Scene&);
    bool hasRecentOctrees    () const;
    void forEachRecentOctree (const std::function <void (const Mesh&, const Octree&)>) const;

  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif
