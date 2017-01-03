/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
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
#include "util.hpp"
#include "winged/mesh.hpp"

namespace {
  struct SnapshotConfig {
    bool snapshotWingedMeshes;
    bool snapshotSketchMeshes;

    SnapshotConfig (bool w, bool s)
      : snapshotWingedMeshes (w)
      , snapshotSketchMeshes (s)
    {
      assert (this->snapshotWingedMeshes || this->snapshotSketchMeshes);
    }
  };

  struct SceneSnapshot {
    const SnapshotConfig   config;
    std::list <WingedMesh> wingedMeshes;
    std::list <SketchMesh> sketchMeshes;

    SceneSnapshot (const SnapshotConfig& c) : config (c) {}
  };

  typedef std::list <SceneSnapshot> Timeline;

  SceneSnapshot sceneSnapshot (const Scene& scene, const SnapshotConfig& config) {
    SceneSnapshot snapshot (config);

    if (config.snapshotWingedMeshes) {
      scene.forEachConstMesh ([&config, &snapshot] (const WingedMesh& mesh) {
        snapshot.wingedMeshes.emplace_back (mesh, false);
      });
    }
    if (config.snapshotSketchMeshes) {
      scene.forEachConstMesh ([&config, &snapshot] (const SketchMesh& mesh) {
        snapshot.sketchMeshes.emplace_back (mesh);
      });
    }
    return snapshot;
  }

  void resetToSnapshot (const SceneSnapshot& snapshot, State& state) {
    Scene& scene = state.scene ();

    if (snapshot.config.snapshotWingedMeshes) {
      scene.deleteWingedMeshes ();

      for (const WingedMesh& mesh : snapshot.wingedMeshes) {
        scene.newWingedMesh (state.config (), mesh);
      }
    }
    if (snapshot.config.snapshotSketchMeshes) {
      scene.deleteSketchMeshes ();

      for (const SketchMesh& mesh : snapshot.sketchMeshes) {
        scene.newSketchMesh (state.config (), mesh);
      }
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
    this->snapshot (scene, SnapshotConfig (true, true));
  }

  void snapshotWingedMeshes (const Scene& scene) {
    this->snapshot (scene, SnapshotConfig (true, false));
  }

  void snapshotSketchMeshes (const Scene& scene) {
    this->snapshot (scene, SnapshotConfig (false, true));
  }

  void snapshot (const Scene& scene, const SnapshotConfig& config) {
    assert (undoDepth > 0);

    this->future.clear ();

    while (this->past.size () >= this->undoDepth) {
      this->past.pop_back ();
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
      const SnapshotConfig& config = this->past.front ().config;

      this->future.push_front (std::move (sceneSnapshot (state.scene (), config)));
      resetToSnapshot (this->past.front (), state);
      this->past.pop_front ();
    }
  }

  void redo (State& state) {
    if (this->future.empty () == false) {
      const SnapshotConfig& config = this->future.front ().config;

      this->past.push_front (std::move (sceneSnapshot (state.scene (), config)));
      resetToSnapshot (this->future.front (), state);
      this->future.pop_front ();
    }
  }

  bool hasRecentWingedMesh () const {
    return this->past.empty () == false && this->past.front ().config.snapshotWingedMeshes;
  }

  void forEachRecentWingedMesh (const std::function <void (const WingedMesh&)>& f) const {
    assert (this->hasRecentWingedMesh ());

    for (const WingedMesh& m : this->past.front ().wingedMeshes) {
      f (m);
    }
  }

  const Mesh& meshSnapshot (unsigned int index) const {
    for (const WingedMesh& mesh : this->past.front ().wingedMeshes) {
      if (mesh.index () == index) {
        return mesh.mesh ();
      }
    }
    DILAY_IMPOSSIBLE;
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
DELEGATE1       (void,        History, snapshotAll, const Scene&)
DELEGATE1       (void,        History, snapshotWingedMeshes, const Scene&)
DELEGATE1       (void,        History, snapshotSketchMeshes, const Scene&)
DELEGATE        (void,        History, dropSnapshot)
DELEGATE1       (void,        History, undo, State&)
DELEGATE1       (void,        History, redo, State&)
DELEGATE_CONST  (bool,        History, hasRecentWingedMesh)
DELEGATE1_CONST (void,        History, forEachRecentWingedMesh, const std::function <void (const WingedMesh&)>&)
DELEGATE1_CONST (const Mesh&, History, meshSnapshot, unsigned int)
DELEGATE        (void,        History, reset)
DELEGATE1       (void,        History, runFromConfig, const Config&)
