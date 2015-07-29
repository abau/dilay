/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <list>
#include <vector>
#include "config.hpp"
#include "history.hpp"
#include "maybe.hpp"
#include "mesh.hpp"
#include "octree.hpp"
#include "scene.hpp"
#include "state.hpp"
#include "winged/mesh.hpp"

namespace {
  struct MeshSnapshot {
    Mesh           mesh;
    Maybe <Octree> octree;
  };

  typedef std::list <MeshSnapshot>  SceneSnapshot;
  typedef std::list <SceneSnapshot> Timeline;

  SceneSnapshot sceneSnapshot (const Scene& scene, bool copyOctree) {
    SceneSnapshot snapshot;

    scene.forEachConstMesh ([copyOctree, &snapshot] (const WingedMesh& mesh) {
      if (copyOctree) {
        std::vector <unsigned int> newFaceIndices;

        MeshSnapshot meshSnapshot = { mesh.makePrunedMesh (&newFaceIndices)
                                    , mesh.octree () };

        if (newFaceIndices.empty () == false) {
          meshSnapshot.octree->rewriteIndices (newFaceIndices);
        }
        snapshot.push_front (std::move (meshSnapshot));
      }
      else {
        snapshot.push_front ({ mesh.makePrunedMesh (), Maybe <Octree> () });
      }
    });
    return snapshot;
  }

  void resetToSnapshot (const SceneSnapshot& snapshot, State& state) {
    Scene&      scene         = state.scene ();
    std::string sceneFileName = scene.fileName ();
    scene.reset    ();
    scene.fileName (sceneFileName);

    for (const MeshSnapshot& meshSnapshot : snapshot) {
      scene.newWingedMesh (state.config (), meshSnapshot.mesh);
    }
  }

  void deleteOctreeSnapshot (SceneSnapshot& sceneSnapshot) {
    for (MeshSnapshot& meshSnapshot : sceneSnapshot) {
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

  void snapshot (const Scene& scene) {
    if (scene.isEmpty () == false) {
      this->future.clear ();

      while (this->past.size () >= this->undoDepth) {
        this->past.pop_back ();
      }
      if (this->past.empty () == false) {
        deleteOctreeSnapshot (this->past.front ());
      }
      this->past.push_front (std::move (sceneSnapshot (scene, true)));
    }
  }

  void dropSnapshot () {
    if (this->past.empty () == false) {
      this->past.pop_front ();
    }
  }

  void undo (State& state) {
    if (this->past.empty () == false) {
      this->future.push_front (std::move (sceneSnapshot (state.scene (), false)));
      resetToSnapshot (this->past.front (), state);
      this->past.pop_front ();
    }
  }

  void redo (State& state) {
    if (this->future.empty () == false) {
      if (this->past.empty () == false) {
        deleteOctreeSnapshot (this->past.front ());
      }
      this->past.push_front (std::move (sceneSnapshot (state.scene (), false)));
      resetToSnapshot (this->future.front (), state);
      this->future.pop_front ();
    }
  }

  bool hasRecentOctrees () const {
    return this->past.empty () == false && bool (this->past.front ().front ().octree);
  }

  void forEachRecentOctree (const std::function <void (const Mesh& m, const Octree&)> f) const {
    assert (this->hasRecentOctrees ());
    for (const MeshSnapshot& s : this->past.front ()) {
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
DELEGATE1       (void, History, snapshot, const Scene&)
DELEGATE        (void, History, dropSnapshot)
DELEGATE1       (void, History, undo, State&)
DELEGATE1       (void, History, redo, State&)
DELEGATE1       (void, History, runFromConfig, const Config&)
DELEGATE_CONST  (bool, History, hasRecentOctrees)
DELEGATE        (void, History, reset)
DELEGATE1_CONST (void, History, forEachRecentOctree, const std::function <void (const Mesh&, const Octree&)>)
