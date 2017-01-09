/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include <list>
#include "config.hpp"
#include "dynamic/mesh-intersection.hpp"
#include "dynamic/mesh.hpp"
#include "intersection.hpp"
#include "render-mode.hpp"
#include "scene.hpp"
#include "scene-util.hpp"
#include "sketch/mesh.hpp"
#include "sketch/bone-intersection.hpp"
#include "sketch/mesh-intersection.hpp"
#include "sketch/node-intersection.hpp"
#include "sketch/path-intersection.hpp"
#include "util.hpp"

struct Scene :: Impl {
  Scene*                  self;
  std::list <DynamicMesh> dynamicMeshes;
  std::list <SketchMesh>  sketchMeshes;
  RenderMode              commonRenderMode;
  std::string             fileName;

  Impl (Scene* s, const Config& config)
    : self (s)
  {
    this->runFromConfig (config);

    this->commonRenderMode.smoothShading (true);
  }

  DynamicMesh& newDynamicMesh (const Config& config, const DynamicMesh& other) {
    this->dynamicMeshes.emplace_back (other);
    return this->setupNewMesh (config, this->dynamicMeshes.back ());
  }

  DynamicMesh& newDynamicMesh (const Config& config, const Mesh& mesh) {
    this->dynamicMeshes.emplace_back (mesh);
    return this->setupNewMesh (config, this->dynamicMeshes.back ());
  }

  DynamicMesh& setupNewMesh (const Config& config, DynamicMesh& mesh) {
    mesh.bufferData ();
    mesh.renderMode () = this->commonRenderMode;
    mesh.fromConfig (config);
    return mesh;
  }

  SketchMesh& newSketchMesh (const Config& config, const SketchMesh& other) {
    this->sketchMeshes.emplace_back (other);
    return this->setupNewMesh (config, this->sketchMeshes.back ());
  }

  SketchMesh& newSketchMesh (const Config& config, const SketchTree& tree) {
    this->sketchMeshes.emplace_back ();
    this->sketchMeshes.back ().fromTree (tree);
    return this->setupNewMesh (config, this->sketchMeshes.back ());
  }

  SketchMesh& setupNewMesh (const Config& config, SketchMesh& mesh) {
    mesh.renderWireframe (this->commonRenderMode.renderWireframe ());
    mesh.fromConfig (config);
    return mesh;
  }

  void deleteMesh (DynamicMesh& mesh) {
    for (auto it = this->dynamicMeshes.begin (); it != this->dynamicMeshes.end (); ++it) {
      if (&*it == &mesh) {
        this->dynamicMeshes.erase (it);
        this->resetIfEmpty ();
        return;
      }
    }
    DILAY_IMPOSSIBLE
  }

  void deleteMesh (SketchMesh& mesh) {
    for (auto it = this->sketchMeshes.begin (); it != this->sketchMeshes.end (); ++it) {
      if (&*it == &mesh) {
        this->sketchMeshes.erase (it);
        this->resetIfEmpty ();
        return;
      }
    }
    DILAY_IMPOSSIBLE
  }

  void deleteDynamicMeshes () {
    this->dynamicMeshes.clear ();
  }

  void deleteSketchMeshes () {
    this->sketchMeshes.clear ();
  }

  void deleteEmptyMeshes () {
    for (auto it = this->dynamicMeshes.begin (); it != this->dynamicMeshes.end (); ) {
      if (it->isEmpty ()) {
        it = this->dynamicMeshes.erase (it);
      }
      else {
        ++it;
      }
    }
    for (auto it = this->sketchMeshes.begin (); it != this->sketchMeshes.end (); ) {
      if (it->isEmpty ()) {
        it = this->sketchMeshes.erase (it);
      }
      else {
        ++it;
      }
    }
    this->resetIfEmpty ();
  }

  void render (Camera& camera) {
    this->forEachMesh ([&] (DynamicMesh& m) {
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

  bool intersects (const PrimRay& ray, DynamicMeshIntersection& intersection) {
    return this->intersectsT <DynamicMesh> (ray, intersection);
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
    DynamicMeshIntersection dIntersection;
    SketchMeshIntersection sIntersection;

    if (this->intersects (ray, dIntersection)) {
      intersection.update ( dIntersection.distance ()
                          , dIntersection.position ()
                          , dIntersection.normal   () );
    }
    if (this->intersects (ray, sIntersection)) {
      intersection.update ( sIntersection.distance ()
                          , sIntersection.position ()
                          , sIntersection.normal   () );
    }
    return intersection.isIntersection ();
  }

  void printStatistics () const {
    this->forEachConstMesh ([] (const DynamicMesh& mesh) {
      mesh.printStatistics ();
    });
  }

  void forEachMesh (const std::function <void (DynamicMesh&)>& f) {
    for (DynamicMesh& m : this->dynamicMeshes) {
      f (m);
    }
  }

  void forEachMesh (const std::function <void (SketchMesh&)>& f) {
    for (SketchMesh& m : this->sketchMeshes) {
      f (m);
    }
  }

  void forEachConstMesh (const std::function <void (const DynamicMesh&)>& f) const {
    for (const DynamicMesh& m : this->dynamicMeshes) {
      f (m);
    }
  }

  void forEachConstMesh (const std::function <void (const SketchMesh&)>& f) const {
    for (const SketchMesh& m : this->sketchMeshes) {
      f (m);
    }
  }

  void sanitizeMeshes () {
    this->forEachMesh ([] (DynamicMesh& mesh) {
      mesh.sanitize ();
    });
  }

  void reset () {
    this->deleteDynamicMeshes ();
    this->deleteSketchMeshes ();
    this->fileName.clear ();
  }

  void resetIfEmpty () {
    if (this->isEmpty ()) {
      this->reset ();
    }
  }

  void setCommonRenderMode (const RenderMode& mode) {
    this->commonRenderMode = mode;
    this->forEachMesh ([this] (DynamicMesh& mesh) {
      mesh.renderMode () = this->commonRenderMode; 
    });
    this->forEachMesh ([&mode] (SketchMesh& mesh) {
      mesh.renderWireframe (mode.renderWireframe ()); 
    });
  }

  bool renderWireframe () const {
    return this->commonRenderMode.renderWireframe ();
  }

  void renderWireframe (bool value) {
    this->commonRenderMode.renderWireframe (value);
    this->setCommonRenderMode (this->commonRenderMode);
  }

  void toggleWireframe () {
    this->renderWireframe (! this->commonRenderMode.renderWireframe ());
  }

  void toggleShading () {
    if (this->commonRenderMode.smoothShading ()) {
      this->commonRenderMode.flatShading (true);
    }
    else if (this->commonRenderMode.flatShading ()) {
      this->commonRenderMode.smoothShading (true);
    }
    this->setCommonRenderMode (this->commonRenderMode);
  }

  bool isEmpty () const {
    return this->numDynamicMeshes () == 0 && this->numSketchMeshes () == 0;
  }

  unsigned int numDynamicMeshes () const {
    return this->dynamicMeshes.size ();
  }

  unsigned int numSketchMeshes () const {
    return this->sketchMeshes.size ();
  }

  unsigned int numFaces () const {
    unsigned int n = 0;
    this->forEachConstMesh ([this, &n] (const DynamicMesh& mesh) {
      n += mesh.numFaces ();
    });
    return n;
  }

  bool hasFileName () const {
    return ! this->fileName.empty ();
  }

  bool toDlyFile (bool isObjFile) {
    assert (this->hasFileName ());

    return Util::withCLocale <bool> ([this, isObjFile] () {
      if (SceneUtil::toDlyFile (this->fileName, *this->self, isObjFile)) {
        return true;
      }
      else {
        this->fileName.clear ();
        return false;
      }
    });
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
  
  void runFromConfig (const Config& config) {
    this->forEachMesh ([this, &config] (DynamicMesh& mesh) {
      mesh.fromConfig (config);
    });
    this->forEachMesh ([this, &config] (SketchMesh& mesh) {
      mesh.fromConfig (config);
    });
  }
};

DELEGATE1_BIG3_SELF (Scene, const Config&)

DELEGATE2       (DynamicMesh&      , Scene, newDynamicMesh, const Config&, const DynamicMesh&)
DELEGATE2       (DynamicMesh&      , Scene, newDynamicMesh, const Config&, const Mesh&)
DELEGATE2       (SketchMesh&       , Scene, newSketchMesh, const Config&, const SketchMesh&)
DELEGATE2       (SketchMesh&       , Scene, newSketchMesh, const Config&, const SketchTree&)
DELEGATE1       (void              , Scene, deleteMesh, DynamicMesh&)
DELEGATE1       (void              , Scene, deleteMesh, SketchMesh&)
DELEGATE        (void              , Scene, deleteDynamicMeshes)
DELEGATE        (void              , Scene, deleteSketchMeshes)
DELEGATE        (void              , Scene, deleteEmptyMeshes)
DELEGATE1       (void              , Scene, render, Camera&)
DELEGATE2       (bool              , Scene, intersects, const PrimRay&, DynamicMeshIntersection&)
DELEGATE2       (bool              , Scene, intersects, const PrimRay&, SketchNodeIntersection&)
DELEGATE2       (bool              , Scene, intersects, const PrimRay&, SketchBoneIntersection&)
DELEGATE2       (bool              , Scene, intersects, const PrimRay&, SketchMeshIntersection&)
DELEGATE3       (bool              , Scene, intersects, const PrimRay&, SketchMeshIntersection&, unsigned int)
DELEGATE2       (bool              , Scene, intersects, const PrimRay&, SketchPathIntersection&)
DELEGATE2       (bool              , Scene, intersects, const PrimRay&, Intersection&)
DELEGATE_CONST  (void              , Scene, printStatistics)
DELEGATE1       (void              , Scene, forEachMesh, const std::function <void (DynamicMesh&)>&)
DELEGATE1       (void              , Scene, forEachMesh, const std::function <void (SketchMesh&)>&)
DELEGATE1_CONST (void              , Scene, forEachConstMesh, const std::function <void (const DynamicMesh&)>&)
DELEGATE1_CONST (void              , Scene, forEachConstMesh, const std::function <void (const SketchMesh&)>&)
DELEGATE        (void              , Scene, sanitizeMeshes)
DELEGATE        (void              , Scene, reset)
DELEGATE_CONST  (bool              , Scene, renderWireframe)
DELEGATE1       (void              , Scene, renderWireframe, bool)
DELEGATE        (void              , Scene, toggleWireframe)
DELEGATE        (void              , Scene, toggleShading)
DELEGATE_CONST  (bool              , Scene, isEmpty)
DELEGATE_CONST  (unsigned int      , Scene, numDynamicMeshes)
DELEGATE_CONST  (unsigned int      , Scene, numSketchMeshes)
DELEGATE_CONST  (unsigned int      , Scene, numFaces)
DELEGATE_CONST  (bool              , Scene, hasFileName)
GETTER_CONST    (const std::string&, Scene, fileName)
DELEGATE1       (bool              , Scene, toDlyFile, bool)
DELEGATE2       (bool              , Scene, toDlyFile, const std::string&, bool)
DELEGATE2       (bool              , Scene, fromDlyFile, const Config&, const std::string&)
DELEGATE1       (void              , Scene, runFromConfig, const Config&)
