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

  Impl (const ConfigProxy& wmConfig)
    : wingedMeshConfig (wmConfig)
  {}

  WingedMesh& newWingedMesh () {
    return this->wingedMeshes.emplace ();
  }

  WingedMesh& newWingedMesh (unsigned int index) {
    return this->wingedMeshes.emplaceAt (index);
  }

  void deleteMesh (WingedMesh& mesh) {
    return this->wingedMeshes.deleteElement (mesh);
  }

  WingedMesh* wingedMesh (unsigned int index) const { 
    return this->wingedMeshes.get (index); 
  }

  void render (Camera& camera) {
    const Color& normalColor = this->wingedMeshConfig.get <Color> ("color/normal");
    const Color& wireColor   = this->wingedMeshConfig.get <Color> ("color/wireframe");

    this->forEachMesh ([&] (WingedMesh& m) {
      m.color          (normalColor);
      m.wireframeColor (wireColor);
      m.render         (camera);
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
};

DELEGATE1_BIG3 (Scene, const ConfigProxy&)

DELEGATE        (WingedMesh&      , Scene, newWingedMesh)
DELEGATE1       (WingedMesh&      , Scene, newWingedMesh, unsigned int)
DELEGATE1       (void             , Scene, deleteMesh, WingedMesh&)
DELEGATE1_CONST (WingedMesh*      , Scene, wingedMesh, unsigned int)
DELEGATE1       (void             , Scene, render, Camera&)
DELEGATE2       (bool             , Scene, intersects, const PrimRay&, WingedFaceIntersection&)
DELEGATE1_CONST (void             , Scene, printStatistics, bool)
DELEGATE1       (void             , Scene, forEachMesh, const std::function <void (WingedMesh&)>&)
DELEGATE1_CONST (void             , Scene, forEachConstMesh, const std::function <void (const WingedMesh&)>&)
DELEGATE_CONST  (const WingedMesh*, Scene, someWingedMesh)
