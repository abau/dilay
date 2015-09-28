/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_HISTORY
#define DILAY_HISTORY

#include "configurable.hpp"
#include "macro.hpp"

class IndexOctree;
class Mesh;
class Scene;
class State;

class History : public Configurable {
  public: 
    DECLARE_BIG3 (History, const Config&)

    void snapshotAll          (const Scene&);
    void snapshotWingedMeshes (const Scene&);
    void snapshotSketchMeshes (const Scene&);
    void dropSnapshot         ();
    void undo                 (State&);
    void redo                 (State&);
    bool hasRecentOctrees     () const;
    void forEachRecentOctree  (const std::function <void (const Mesh&, const IndexOctree&)>&) const;
    void reset                ();

  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif
