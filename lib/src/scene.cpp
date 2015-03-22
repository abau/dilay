#include "config.hpp"
#include "indexable.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
#include "winged/face-intersection.hpp"
#include "winged/mesh.hpp"
#include "winged/util.hpp"

struct Scene :: Impl {
  const ConfigProxy          wingedMeshConfig;
  IndexableList <WingedMesh> wingedMeshes;

  Impl (const Config& config) 
    : wingedMeshConfig (ConfigProxy (config, "editor/mesh/"))
  {
    this->runFromConfig (config);
  }

  WingedMesh& newWingedMesh () {
    return this->wingedMeshes.emplace ();
  }

  WingedMesh& newWingedMesh (const Mesh& mesh) {
    WingedMesh& wingedMesh = this->newWingedMesh ();
    wingedMesh.fromMesh (mesh);
    wingedMesh.bufferData ();

    this->runFromConfig (wingedMesh);
    return wingedMesh;
  }

  void deleteMesh (WingedMesh& mesh) {
    return this->wingedMeshes.deleteElement (mesh);
  }

  WingedMesh* wingedMesh (unsigned int index) const { 
    return this->wingedMeshes.get (index); 
  }

  void render (Camera& camera) {
    this->forEachMesh ([&] (WingedMesh& m) {
      m.render (camera);
    });
  }

  bool intersects (const PrimRay& ray, WingedFaceIntersection& intersection) {
    this->forEachMesh ([this, &ray, &intersection] (WingedMesh& m) {
      m.intersects (ray, intersection);
    });
    return intersection.isIntersection ();
  }

  void printStatistics (bool printAll) const {
    this->forEachConstMesh ([printAll] (const WingedMesh& m) {
      WingedUtil::printStatistics (m, printAll);
    });
  }

  void forEachMesh (const std::function <void (WingedMesh&)>& f) {
    this->wingedMeshes.forEachElement (f);
  }

  void forEachConstMesh (const std::function <void (const WingedMesh&)>& f) const {
    this->wingedMeshes.forEachConstElement (f);
  }

  const WingedMesh* someWingedMesh () const {
    return this->wingedMeshes.getSome ();
  }

  void reset () {
    this->wingedMeshes.reset ();
  }

  void runFromConfig (WingedMesh& mesh) {
    mesh.color          (this->wingedMeshConfig.get <Color> ("color/normal"));
    mesh.wireframeColor (this->wingedMeshConfig.get <Color> ("color/wireframe"));
  }

  void runFromConfig (const Config& config) {
    assert (&config == &this->wingedMeshConfig.config ());

    this->forEachMesh ([this] (WingedMesh& mesh) {
      this->runFromConfig (mesh);
    });
  }
};

DELEGATE1_BIG3 (Scene, const Config&)

DELEGATE        (WingedMesh&      , Scene, newWingedMesh)
DELEGATE1       (WingedMesh&      , Scene, newWingedMesh, const Mesh&)
DELEGATE1       (void             , Scene, deleteMesh, WingedMesh&)
DELEGATE1_CONST (WingedMesh*      , Scene, wingedMesh, unsigned int)
DELEGATE1       (void             , Scene, render, Camera&)
DELEGATE2       (bool             , Scene, intersects, const PrimRay&, WingedFaceIntersection&)
DELEGATE1_CONST (void             , Scene, printStatistics, bool)
DELEGATE1       (void             , Scene, forEachMesh, const std::function <void (WingedMesh&)>&)
DELEGATE1_CONST (void             , Scene, forEachConstMesh, const std::function <void (const WingedMesh&)>&)
DELEGATE_CONST  (const WingedMesh*, Scene, someWingedMesh)
DELEGATE        (void             , Scene, reset)
DELEGATE1       (void             , Scene, runFromConfig, const Config&)
