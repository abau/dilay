/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <list>
#include <vector>
#include "config.hpp"
#include "history.hpp"
#include "index-octree.hpp"
#include "maybe.hpp"
#include "mesh.hpp"
#include "scene.hpp"
#include "sketch/mesh.hpp"
#include "sketch/path.hpp"
#include "state.hpp"
#include "winged/mesh.hpp"

namespace {
  struct SnapshotConfig {
    bool snapshotWingedMeshes;
    bool copyOctree;
    bool snapshotSketchMeshes;

    SnapshotConfig (bool w, bool o, bool s)
      : snapshotWingedMeshes (w)
      , copyOctree           (o)
      , snapshotSketchMeshes (s)
    {
      assert (!this->copyOctree || this->snapshotWingedMeshes);
    }

    SnapshotConfig withoutOctrees () const {
      return SnapshotConfig ( this->snapshotWingedMeshes
                            , false
                            , this->snapshotSketchMeshes );
    }
  };

  struct WingedMeshSnapshot {
    Mesh                mesh;
    Maybe <IndexOctree> octree;
  };

  struct SketchMeshSnapshot {
    SketchTree  tree;
    SketchPaths paths;
  };

  struct SceneSnapshot {
    const SnapshotConfig           config;
    std::list <WingedMeshSnapshot> wingedMeshes;
    std::list <SketchMeshSnapshot> sketchMeshes;

    SceneSnapshot (const SnapshotConfig& c) : config (c) {}
  };

  typedef std::list <SceneSnapshot> Timeline;

  SceneSnapshot sceneSnapshot (const Scene& scene, const SnapshotConfig& config) {
    SceneSnapshot snapshot (config);

    if (config.snapshotWingedMeshes) {
      scene.forEachConstMesh ([&config, &snapshot] (const WingedMesh& mesh) {
        if (config.copyOctree) {
          std::vector <unsigned int> newFaceIndices;

          WingedMeshSnapshot meshSnapshot = { mesh.makePrunedMesh (&newFaceIndices)
                                            , mesh.octree () };

          if (newFaceIndices.empty () == false) {
            meshSnapshot.octree->rewriteIndices (newFaceIndices);
          }
          snapshot.wingedMeshes.push_back (std::move (meshSnapshot));
        }
        else {
          snapshot.wingedMeshes.push_back ({ mesh.makePrunedMesh (), Maybe <IndexOctree> () });
        }
      });
    }
    if (config.snapshotSketchMeshes) {
      scene.forEachConstMesh ([&config, &snapshot] (const SketchMesh& mesh) {
        snapshot.sketchMeshes.push_back ({ mesh.tree (), mesh.paths () });
      });
    }
    return snapshot;
  }

  void resetToSnapshot (const SceneSnapshot& snapshot, State& state) {
    Scene& scene = state.scene ();

    if (snapshot.config.snapshotWingedMeshes) {
      scene.deleteWingedMeshes ();

      for (const WingedMeshSnapshot& meshSnapshot : snapshot.wingedMeshes) {
        scene.newWingedMesh (state.config (), meshSnapshot.mesh);
      }
    }
    if (snapshot.config.snapshotSketchMeshes) {
      scene.deleteSketchMeshes ();

      for (const SketchMeshSnapshot& meshSnapshot : snapshot.sketchMeshes) {
        SketchMesh& sketch = scene.newSketchMesh (state.config (), meshSnapshot.tree);
        for (const SketchPath& p : meshSnapshot.paths) {
          sketch.addPath (p);
        }
      }
    }
  }

  void deleteOctreeSnapshot (SceneSnapshot& sceneSnapshot) {
    for (WingedMeshSnapshot& meshSnapshot : sceneSnapshot.wingedMeshes) {
      meshSnapshot.octree.reset ();
    }
  }
}

struct History::Impl {
  unsigned int undoDepth;
  Timeline     past;
  Timeline     future;

  Impl (const Config& config) {
    this->runFromConfig (config);
  }

  void snapshotAll (const Scene& scene) {
    this->snapshot (scene, SnapshotConfig (true, true, true));
  }

  void snapshotWingedMeshes (const Scene& scene) {
    this->snapshot (scene, SnapshotConfig (true, true, false));
  }

  void snapshotSketchMeshes (const Scene& scene) {
    this->snapshot (scene, SnapshotConfig (false, false, true));
  }

  void snapshot (const Scene& scene, const SnapshotConfig& config) {
    this->future.clear ();

    while (this->past.size () >= this->undoDepth) {
      this->past.pop_back ();
    }
    if (this->past.empty () == false) {
      deleteOctreeSnapshot (this->past.front ());
    }
    this->past.push_front (std::move (sceneSnapshot (scene, config)));
  }

  void dropSnapshot () {
    if (this->past.empty () == false) {
      this->past.pop_front ();
    }
  }

  void undo (State& state) {
    if (this->past.empty () == false) {
      SnapshotConfig config = this->past.front ().config.withoutOctrees ();

      this->future.push_front (std::move (sceneSnapshot (state.scene (), config)));
      resetToSnapshot (this->past.front (), state);
      this->past.pop_front ();
    }
  }

  void redo (State& state) {
    if (this->future.empty () == false) {
      if (this->past.empty () == false) {
        deleteOctreeSnapshot (this->past.front ());
      }
      SnapshotConfig config = this->future.front ().config.withoutOctrees ();

      this->past.push_front (std::move (sceneSnapshot (state.scene (), config)));
      resetToSnapshot (this->future.front (), state);
      this->future.pop_front ();
    }
  }

  bool hasRecentOctrees () const {
    return this->past.empty () == false 
      && this->past.front ().config.snapshotWingedMeshes
      && bool (this->past.front ().wingedMeshes.front ().octree);
  }

  void forEachRecentOctree (const std::function <void ( const Mesh& m
                                                      , const IndexOctree& )>& f) const
  {
    assert (this->hasRecentOctrees ());
    for (const WingedMeshSnapshot& s : this->past.front ().wingedMeshes) {
      assert (s.octree);
      f (s.mesh, *s.octree);
    }
  }

  void reset () {
    this->past  .clear ();
    this->future.clear ();
  }

  void runFromConfig (const Config& config) {
    this->undoDepth = config.get <int> ("editor/undo-depth");
  }
};

DELEGATE1_BIG3  (History, const Config&)
DELEGATE1       (void, History, snapshotAll, const Scene&)
DELEGATE1       (void, History, snapshotWingedMeshes, const Scene&)
DELEGATE1       (void, History, snapshotSketchMeshes, const Scene&)
DELEGATE        (void, History, dropSnapshot)
DELEGATE1       (void, History, undo, State&)
DELEGATE1       (void, History, redo, State&)
DELEGATE1       (void, History, runFromConfig, const Config&)
DELEGATE_CONST  (bool, History, hasRecentOctrees)
DELEGATE        (void, History, reset)
DELEGATE1_CONST (void, History, forEachRecentOctree, const std::function <void (const Mesh&, const IndexOctree&)>&)
