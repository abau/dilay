/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <fstream>
#include "config.hpp"
#include "indexable.hpp"
#include "mesh.hpp"
#include "mesh-util.hpp"
#include "render-mode.hpp"
#include "scene.hpp"
#include "winged/face-intersection.hpp"
#include "winged/mesh.hpp"
#include "winged/util.hpp"

struct Scene :: Impl {
  IndexableList <WingedMesh> wingedMeshes;
  RenderMode                _commonRenderMode;
  std::string                fileName;

  Impl (const Config& config) {
    this->runFromConfig (config);

    this->_commonRenderMode.smoothShading (true);
  }

  WingedMesh& newWingedMesh (const Config& config, const Mesh& mesh) {
    WingedMesh& wingedMesh = this->wingedMeshes.emplace ();

    wingedMesh.fromMesh (mesh);
    wingedMesh.bufferData ();
    wingedMesh.renderMode () = this->_commonRenderMode;

    this->runFromConfig (config, wingedMesh);
    return wingedMesh;
  }

  void deleteMesh (WingedMesh& mesh) {
    this->wingedMeshes.deleteElement (mesh);
    this->resetIfEmpty ();
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
    this->fileName    .clear ();
  }

  void resetIfEmpty () {
    if (this->isEmpty ()) {
      this->reset ();
    }
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

  bool isEmpty () const {
    return this->numWingedMeshes () == 0;
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

  bool fromObjFile (const Config& config, const std::string& newFileName) {
    this->fileName = newFileName;

    std::ifstream      file (this->fileName);
    std::vector <Mesh> meshes;

    if (file.is_open () && MeshUtil::fromObjFile (file, meshes)) {
      for (Mesh& m : meshes) {
        this->newWingedMesh (config, m);
      }
      file.close ();
      return true;
    }
    else {
      this->fileName.clear ();
      return false;
    }
  }
  
  void runFromConfig (const Config& config, WingedMesh& mesh) {
    ConfigProxy wingedMeshConfig (config, "editor/mesh/");

    mesh.color          (wingedMeshConfig.get <Color> ("color/normal"));
    mesh.wireframeColor (wingedMeshConfig.get <Color> ("color/wireframe"));
  }

  void runFromConfig (const Config& config) {
    this->forEachMesh ([this, &config] (WingedMesh& mesh) {
      this->runFromConfig (config, mesh);
    });
  }
};

DELEGATE1_BIG3 (Scene, const Config&)

DELEGATE2       (WingedMesh&       , Scene, newWingedMesh, const Config&, const Mesh&)
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
DELEGATE_CONST  (bool              , Scene, isEmpty)
DELEGATE_CONST  (unsigned int      , Scene, numWingedMeshes)
DELEGATE_CONST  (unsigned int      , Scene, numFaces)
DELEGATE        (void              , Scene, deleteEmptyMeshes)
DELEGATE_CONST  (bool              , Scene, hasFileName)
GETTER_CONST    (const std::string&, Scene, fileName)
SETTER          (const std::string&, Scene, fileName)
DELEGATE        (bool              , Scene, toObjFile)
DELEGATE1       (bool              , Scene, toObjFile, const std::string&)
DELEGATE2       (bool              , Scene, fromObjFile, const Config&, const std::string&)
DELEGATE1       (void              , Scene, runFromConfig, const Config&)
