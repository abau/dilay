#include <fstream>
#include "config.hpp"
#include "indexable.hpp"
#include "mesh.hpp"
#include "mesh-util.hpp"
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
  std::string                fileName;

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
    this->fileName.clear     ();
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

  unsigned int numFaces () const {
    unsigned int n = 0;
    this->forEachConstMesh ([this, &n] (const WingedMesh& mesh) {
      n = n + mesh.numFaces ();
    });
    return n;
  }

  void deleteEmptyMeshes () {
    this->forEachMesh ([this] (WingedMesh& mesh) {
      if (mesh.numFaces () == 0) {
        this->deleteMesh (mesh);
      }
    });
  }

  bool hasFileName () const {
    return ! this->fileName.empty ();
  }

  bool toObjFile () {
    assert (this->hasFileName ());

    std::ofstream file (this->fileName);

    if (file.is_open ()) {
      this->forEachConstMesh ([&file] (const WingedMesh& mesh) {
        file << "o object" << mesh.index () << std::endl;
        MeshUtil::toObjFile (file, mesh.makePrunedMesh ());
      });
      file.close ();
      return true;
    }
    else {
      this->fileName.clear ();
      return false;
    }
  }

  bool toObjFile (const std::string& newFileName) {
    this->fileName = newFileName;
    return this->toObjFile ();
  }

  bool fromObjFile (const std::string& newFileName) {
    this->fileName = newFileName;

    std::ifstream      file (this->fileName);
    std::vector <Mesh> meshes;

    if (file.is_open () && MeshUtil::fromObjFile (file, meshes)) {
      for (Mesh& m : meshes) {
        this->newWingedMesh (m);
      }
      file.close ();
      return true;
    }
    else {
      this->fileName.clear ();
      return false;
    }
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

DELEGATE        (WingedMesh&       , Scene, newWingedMesh)
DELEGATE1       (WingedMesh&       , Scene, newWingedMesh, const Mesh&)
DELEGATE1       (void              , Scene, deleteMesh, WingedMesh&)
DELEGATE1_CONST (WingedMesh*       , Scene, wingedMesh, unsigned int)
DELEGATE1       (void              , Scene, render, Camera&)
DELEGATE2       (bool              , Scene, intersects, const PrimRay&, WingedFaceIntersection&)
DELEGATE1_CONST (void              , Scene, printStatistics, bool)
DELEGATE1       (void              , Scene, forEachMesh, const std::function <void (WingedMesh&)>&)
DELEGATE1_CONST (void              , Scene, forEachConstMesh, const std::function <void (const WingedMesh&)>&)
DELEGATE        (void              , Scene, reset)
DELEGATE_CONST  (const RenderMode& , Scene, commonRenderMode)
DELEGATE1       (void              , Scene, commonRenderMode, const RenderMode&)
DELEGATE_CONST  (unsigned int      , Scene, numWingedMeshes)
DELEGATE_CONST  (unsigned int      , Scene, numFaces)
DELEGATE        (void              , Scene, deleteEmptyMeshes)
DELEGATE_CONST  (bool              , Scene, hasFileName)
GETTER_CONST    (const std::string&, Scene, fileName)
DELEGATE        (bool              , Scene, toObjFile)
DELEGATE1       (bool              , Scene, toObjFile, const std::string&)
DELEGATE1       (bool              , Scene, fromObjFile, const std::string&)
DELEGATE1       (void              , Scene, runFromConfig, const Config&)
