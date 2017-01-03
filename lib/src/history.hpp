/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_HISTORY
#define DILAY_HISTORY

#include <functional>
#include "configurable.hpp"
#include "macro.hpp"

class Mesh;
class Scene;
class State;
class WingedMesh;

class History : public Configurable {
  public: 
    DECLARE_BIG3 (History, const Config&)

    void        snapshotAll             (const Scene&);
    void        snapshotWingedMeshes    (const Scene&);
    void        snapshotSketchMeshes    (const Scene&);
    void        dropSnapshot            ();
    void        undo                    (State&);
    void        redo                    (State&);
    bool        hasRecentWingedMesh     () const;
    void        forEachRecentWingedMesh (const std::function <void (const WingedMesh&)>&) const;
    const Mesh& meshSnapshot            (unsigned int) const;
    void        reset                   ();

  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif
