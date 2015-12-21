/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "config.hpp"
#include "intersection.hpp"
#include "render-mode.hpp"
#include "scene.hpp"
#include "scene-util.hpp"
#include "sketch/mesh.hpp"
#include "sketch/bone-intersection.hpp"
#include "sketch/mesh-intersection.hpp"
#include "sketch/node-intersection.hpp"
#include "sketch/path-intersection.hpp"
#include "winged/face-intersection.hpp"
#include "winged/mesh.hpp"
#include "winged/util.hpp"

struct Scene :: Impl {
  Scene*                            self;
  IntrusiveIndexedList <WingedMesh> wingedMeshes;
  IntrusiveIndexedList <SketchMesh> sketchMeshes;
  RenderMode                       _commonRenderMode;
  std::string                       fileName;

  Impl (Scene* s, const Config& config)
    : self (s)
  {
    this->runFromConfig (config);

    this->_commonRenderMode.smoothShading (true);
  }

  WingedMesh& newWingedMesh (const Config& config, const Mesh& mesh) {
    WingedMesh& wingedMesh = this->wingedMeshes.emplaceBack ();

    wingedMesh.fromMesh (mesh);
    wingedMesh.bufferData ();
    wingedMesh.renderMode () = this->_commonRenderMode;

    this->runFromConfig (config, wingedMesh);
    return wingedMesh;
  }

  SketchMesh& newSketchMesh (const Config& config, const SketchTree& tree) {
    SketchMesh& mesh = this->sketchMeshes.emplaceBack ();

    mesh.fromTree        (tree);
    mesh.renderWireframe (this->_commonRenderMode.renderWireframe ());
    mesh.fromConfig      (config);

    return mesh;
  }

  void deleteMesh (WingedMesh& mesh) {
    this->wingedMeshes.deleteElement (mesh);
    this->resetIfEmpty ();
  }

  void deleteMesh (SketchMesh& mesh) {
    this->sketchMeshes.deleteElement (mesh);
    this->resetIfEmpty ();
  }

  void deleteWingedMeshes () {
    this->wingedMeshes.reset ();
  }

  void deleteSketchMeshes () {
    this->sketchMeshes.reset ();
  }

  void deleteEmptyMeshes () {
    this->forEachMesh ([this] (WingedMesh& mesh) {
      if (mesh.numFaces () == 0) {
        this->deleteMesh (mesh);
      }
    });
    this->forEachMesh ([this] (SketchMesh& mesh) {
      if (mesh.isEmpty ()) {
        this->deleteMesh (mesh);
      }
    });
  }

  WingedMesh* wingedMesh (unsigned int index) { 
    return this->wingedMeshes.get (index); 
  }

  SketchMesh* sketchMesh (unsigned int index) { 
    return this->sketchMeshes.get (index); 
  }

  void render (Camera& camera) {
    this->forEachMesh ([&] (WingedMesh& m) {
      m.render (camera);
    });
    this->forEachMesh ([&] (SketchMesh& m) {
      m.render (camera);
    });
  }

  template <typename TMesh, typename TIntersection, typename ... Ts>
  bool intersectsT (const PrimRay& ray, TIntersection& intersection, Ts ... args) {
    this->forEachMesh ([this, &ray, &intersection, &args ...] (TMesh& m) {
      m.intersects (ray, intersection, std::forward <Ts> (args) ...);
    });
    return intersection.isIntersection ();
  }

  bool intersects (const PrimRay& ray, WingedFaceIntersection& intersection) {
    return this->intersectsT <WingedMesh> (ray, intersection);
  }

  bool intersects (const PrimRay& ray, SketchNodeIntersection& intersection) {
    return this->intersectsT <SketchMesh> (ray, intersection);
  }

  bool intersects (const PrimRay& ray, SketchBoneIntersection& intersection) {
    return this->intersectsT <SketchMesh> (ray, intersection);
  }

  bool intersects (const PrimRay& ray, SketchMeshIntersection& intersection) {
    return this->intersectsT <SketchMesh> (ray, intersection);
  }

  bool intersects ( const PrimRay& ray, SketchMeshIntersection& intersection
                  , unsigned int numExcludedLastPaths )
  {
    return this->intersectsT <SketchMesh> (ray, intersection, numExcludedLastPaths);
  }

  bool intersects (const PrimRay& ray, SketchPathIntersection& intersection) {
    return this->intersectsT <SketchMesh> (ray, intersection);
  }

  bool intersects (const PrimRay& ray, Intersection& intersection) {
    WingedFaceIntersection wIntersection;
    SketchMeshIntersection sIntersection;

    if (this->intersects (ray, wIntersection)) {
      intersection.update ( wIntersection.distance ()
                          , wIntersection.position ()
                          , wIntersection.normal   () );
    }
    if (this->intersects (ray, sIntersection)) {
      intersection.update ( sIntersection.distance ()
                          , sIntersection.position ()
                          , sIntersection.normal   () );
    }
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

  void forEachMesh (const std::function <void (SketchMesh&)>& f) {
    this->sketchMeshes.forEachElement (f);
  }

  void forEachConstMesh (const std::function <void (const WingedMesh&)>& f) const {
    this->wingedMeshes.forEachConstElement (f);
  }

  void forEachConstMesh (const std::function <void (const SketchMesh&)>& f) const {
    this->sketchMeshes.forEachConstElement (f);
  }

  void sanitizeMeshes () {
    this->forEachMesh ([] (WingedMesh& mesh) {
      mesh.sanitize ();
    });
  }

  void reset () {
    this->deleteWingedMeshes ();
    this->deleteSketchMeshes ();
    this->fileName.clear ();
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
    this->forEachMesh ([&mode] (SketchMesh& mesh) {
      mesh.renderWireframe (mode.renderWireframe ()); 
    });
  }

  void renderWireframe (bool value) {
    RenderMode mode = this->commonRenderMode ();
    mode.renderWireframe (value);
    this->commonRenderMode (mode);
  }

  void toggleWireframe () {
    this->renderWireframe (! this->commonRenderMode ().renderWireframe ());
  }

  void toggleShading () {
    RenderMode mode = this->commonRenderMode ();

    if (mode.smoothShading ()) {
      mode.flatShading (true);
    }
    else if (mode.flatShading ()) {
      mode.smoothShading (true);
    }
    this->commonRenderMode (mode);
  }

  bool isEmpty () const {
    return this->numWingedMeshes () == 0 && this->numSketchMeshes () == 0;
  }

  unsigned int numWingedMeshes () const {
    return this->wingedMeshes.numElements ();
  }

  unsigned int numSketchMeshes () const {
    return this->sketchMeshes.numElements ();
  }

  unsigned int numFaces () const {
    unsigned int n = 0;
    this->forEachConstMesh ([this, &n] (const WingedMesh& mesh) {
      n = n + mesh.numFaces ();
    });
    return n;
  }

  bool hasFileName () const {
    return ! this->fileName.empty ();
  }

  bool toDlyFile (bool isObjFile) {
    assert (this->hasFileName ());

    if (SceneUtil::toDlyFile (this->fileName, *this->self, isObjFile)) {
      return true;
    }
    else {
      this->fileName.clear ();
      return false;
    }
  }

  bool toDlyFile (const std::string& newFileName, bool isObjFile) {
    this->fileName = newFileName;
    return this->toDlyFile (isObjFile);
  }

  bool fromDlyFile (const Config& config, const std::string& newFileName) {
    this->fileName = newFileName;

    if (SceneUtil::fromDlyFile (this->fileName, config, *this->self)) {
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
    this->forEachMesh ([this, &config] (SketchMesh& mesh) {
      mesh.fromConfig (config);
    });
  }
};

DELEGATE1_BIG3_SELF (Scene, const Config&)

DELEGATE2       (WingedMesh&       , Scene, newWingedMesh, const Config&, const Mesh&)
DELEGATE2       (SketchMesh&       , Scene, newSketchMesh, const Config&, const SketchTree&)
DELEGATE1       (void              , Scene, deleteMesh, WingedMesh&)
DELEGATE1       (void              , Scene, deleteMesh, SketchMesh&)
DELEGATE        (void              , Scene, deleteWingedMeshes)
DELEGATE        (void              , Scene, deleteSketchMeshes)
DELEGATE        (void              , Scene, deleteEmptyMeshes)
DELEGATE1       (WingedMesh*       , Scene, wingedMesh, unsigned int)
DELEGATE1       (SketchMesh*       , Scene, sketchMesh, unsigned int)
DELEGATE1       (void              , Scene, render, Camera&)
DELEGATE2       (bool              , Scene, intersects, const PrimRay&, WingedFaceIntersection&)
DELEGATE2       (bool              , Scene, intersects, const PrimRay&, SketchNodeIntersection&)
DELEGATE2       (bool              , Scene, intersects, const PrimRay&, SketchBoneIntersection&)
DELEGATE2       (bool              , Scene, intersects, const PrimRay&, SketchMeshIntersection&)
DELEGATE3       (bool              , Scene, intersects, const PrimRay&, SketchMeshIntersection&, unsigned int)
DELEGATE2       (bool              , Scene, intersects, const PrimRay&, SketchPathIntersection&)
DELEGATE2       (bool              , Scene, intersects, const PrimRay&, Intersection&)
DELEGATE1_CONST (void              , Scene, printStatistics, bool)
DELEGATE1       (void              , Scene, forEachMesh, const std::function <void (WingedMesh&)>&)
DELEGATE1       (void              , Scene, forEachMesh, const std::function <void (SketchMesh&)>&)
DELEGATE1_CONST (void              , Scene, forEachConstMesh, const std::function <void (const WingedMesh&)>&)
DELEGATE1_CONST (void              , Scene, forEachConstMesh, const std::function <void (const SketchMesh&)>&)
DELEGATE        (void              , Scene, sanitizeMeshes)
DELEGATE        (void              , Scene, reset)
DELEGATE_CONST  (const RenderMode& , Scene, commonRenderMode)
DELEGATE1       (void              , Scene, commonRenderMode, const RenderMode&)
DELEGATE1       (void              , Scene, renderWireframe, bool)
DELEGATE        (void              , Scene, toggleWireframe)
DELEGATE        (void              , Scene, toggleShading)
DELEGATE_CONST  (bool              , Scene, isEmpty)
DELEGATE_CONST  (unsigned int      , Scene, numWingedMeshes)
DELEGATE_CONST  (unsigned int      , Scene, numSketchMeshes)
DELEGATE_CONST  (unsigned int      , Scene, numFaces)
DELEGATE_CONST  (bool              , Scene, hasFileName)
GETTER_CONST    (const std::string&, Scene, fileName)
DELEGATE1       (bool              , Scene, toDlyFile, bool)
DELEGATE2       (bool              , Scene, toDlyFile, const std::string&, bool)
DELEGATE2       (bool              , Scene, fromDlyFile, const Config&, const std::string&)
DELEGATE1       (void              , Scene, runFromConfig, const Config&)
