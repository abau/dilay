#include <list>
#include "config.hpp"
#include "history.hpp"
#include "mesh.hpp"
#include "scene.hpp"
#include "winged/mesh.hpp"

namespace {
  typedef std::list <Mesh>          SceneSnapshot;
  typedef std::list <SceneSnapshot> Timeline;

  SceneSnapshot sceneSnapshot (const Scene& scene) {
    SceneSnapshot snapshot;

    scene.forEachConstMesh ([&snapshot] (const WingedMesh& mesh) {
      snapshot.push_front (mesh.makePrunedMesh ());
    });
    return snapshot;
  }
}

struct History::Impl {
  Timeline     past;
  Timeline     future;
  unsigned int undoDepth;

  Impl (const Config& config) {
    this->runFromConfig (config);
  }

  void snapshot (const Scene& scene) {
    if (scene.numWingedMeshes () > 0) {
      this->future.clear ();

      while (this->past.size () >= this->undoDepth) {
        this->past.pop_back ();
      }
      this->past.push_front (std::move (sceneSnapshot (scene)));
    }
  }

  void dropSnapshot () {
    if (this->past.empty () == false) {
      this->past.pop_front ();
    }
  }

  void resetToSnapshot (const SceneSnapshot& snapshot, Scene& scene) {
    scene.reset ();

    for (const Mesh& mesh : snapshot) {
      scene.newWingedMesh (mesh);
    }
  }

  void undo (Scene& scene) {
    if (this->past.empty () == false) {
      this->future.push_front (std::move (sceneSnapshot (scene)));
      this->resetToSnapshot (this->past.front (), scene);
      this->past.pop_front ();
    }
  }

  void redo (Scene& scene) {
    if (this->future.empty () == false) {
      this->past.push_front (std::move (sceneSnapshot (scene)));
      this->resetToSnapshot (this->future.front (), scene);
      this->future.pop_front ();
    }
  }

  void runFromConfig (const Config& config) {
    this->undoDepth = config.get <int> ("editor/undo-depth");
  }
};

DELEGATE1_BIG3 (History, const Config&)
DELEGATE1 (void, History, snapshot, const Scene&)
DELEGATE  (void, History, dropSnapshot)
DELEGATE1 (void, History, undo, Scene&)
DELEGATE1 (void, History, redo, Scene&)
DELEGATE1 (void, History, runFromConfig, const Config&)
