#include "config.hpp"
#include "indexable.hpp"
#include "primitive/ray.hpp"
#include "render-mode.hpp"
#include "scene.hpp"
#include "winged/face-intersection.hpp"
#include "winged/mesh.hpp"
#include "winged/util.hpp"

struct Scene :: Impl {
  const ConfigProxy          wingedMeshConfig;
  IndexableList <WingedMesh> wingedMeshes;
  RenderMode                _commonRenderMode;

  Impl (const Config& config) 
    : wingedMeshConfig (ConfigProxy (config, "editor/mesh/"))
  {
    this->runFromConfig (config);

    this->_commonRenderMode.smoothShading (true);
  }

  WingedMesh& newWingedMesh () {
    WingedMesh& newWMesh = this->wingedMeshes.emplace ();

    newWMesh.renderMode () = this->_commonRenderMode;
    return newWMesh;
  }

  WingedMesh& newWingedMesh (const Mesh& mesh) {
    WingedMesh& wingedMesh = this->newWingedMesh ();

    wingedMesh.fromMesh (mesh);
    wingedMesh.bufferData ();
    wingedMesh.renderMode () = this->_commonRenderMode;

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

  void reset () {
    this->wingedMeshes.reset ();
  }

  const RenderMode& commonRenderMode () {
    return this->_commonRenderMode;
  }

  void commonRenderMode (const RenderMode& mode) {
    this->_commonRenderMode = mode;
    this->forEachMesh ([&mode] (WingedMesh& mesh) {
      mesh.renderMode () = mode; 
    });
  }

  unsigned int numWingedMeshes () const {
    return this->wingedMeshes.numElements ();
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

  void deleteEmptyMeshes () {
    this->forEachMesh ([this] (WingedMesh& mesh) {
      if (mesh.numFaces () == 0) {
        this->deleteMesh (mesh);
      }
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
DELEGATE        (void             , Scene, reset)
DELEGATE_CONST  (const RenderMode&, Scene, commonRenderMode)
DELEGATE1       (void             , Scene, commonRenderMode, const RenderMode&)
DELEGATE_CONST  (unsigned int     , Scene, numWingedMeshes)
DELEGATE        (void             , Scene, deleteEmptyMeshes)
DELEGATE1       (void             , Scene, runFromConfig, const Config&)
