#include <list>
#include <algorithm>
#include "scene.hpp"
#include "macro.hpp"
#include "id-map.hpp"
#include "winged/mesh.hpp"
#include "ray.hpp"
#include "mesh-type.hpp"
#include "sphere/mesh.hpp"
#include "winged/face-intersection.hpp"
#include "sphere/node-intersection.hpp"

struct Scene :: Impl {
  std::list <WingedMesh> wingedMeshes;
  std::list <SphereMesh> sphereMeshes;

  IdMapPtr <WingedMesh> wingedMeshIdMap;
  IdMapPtr <SphereMesh> sphereMeshIdMap;

  WingedMesh& newWingedMesh (MeshType t) {
    return this->newWingedMesh (t, Id ());
  }

  WingedMesh& newWingedMesh (MeshType t, const Id& id) {
    assert (MeshType::FreeForm == t);
    this->wingedMeshes.emplace_back (id);
    this->wingedMeshIdMap.insert (this->wingedMeshes.back ());
    return this->wingedMeshes.back ();
  }

  void removeWingedMesh (const Id& id) {
    assert (this->wingedMeshIdMap.hasElement (id));
    this->wingedMeshIdMap.erase (id);
    this->wingedMeshes.remove_if ([&id] (WingedMesh& m) { return m.id () == id; });
  }

        WingedMesh& wingedMesh (const Id& id)       { return this->wingedMeshIdMap.elementRef (id); }
  const WingedMesh& wingedMesh (const Id& id) const { return this->wingedMeshIdMap.elementRef (id); }

  SphereMesh& newSphereMesh () {
    return this->newSphereMesh (Id ());
  }

  SphereMesh& newSphereMesh (const Id& id) {
    this->sphereMeshes.emplace_back (id);
    this->sphereMeshIdMap.insert (this->sphereMeshes.back ());
    return this->sphereMeshes.back ();
  }

  void removeSphereMesh (const Id& id) {
    assert (this->sphereMeshIdMap.hasElement (id));
    this->sphereMeshIdMap.erase (id);
    this->sphereMeshes.remove_if ([&id] (SphereMesh& m) { return m.id () == id; });
  }

        SphereMesh& sphereMesh (const Id& id)       { return this->sphereMeshIdMap.elementRef (id); }
  const SphereMesh& sphereMesh (const Id& id) const { return this->sphereMeshIdMap.elementRef (id); }

  void render (MeshType t) {
    if (t == MeshType::FreeForm) {
      for (WingedMesh& m : this->wingedMeshes) {
        m.render ();
      }
    }
    else if (t == MeshType::Sphere) {
      for (SphereMesh& m : this->sphereMeshes) {
        m.render ();
      }
    }
  }

  bool intersects (MeshType t, const Ray& ray, WingedFaceIntersection& intersection) {
    if (t == MeshType::FreeForm) {
      for (WingedMesh& m : this->wingedMeshes) {
        m.intersects (ray, intersection);
      }
    }
    return intersection.isIntersection ();
  }

  bool intersects (const Ray& ray, SphereNodeIntersection& intersection) {
    for (SphereMesh& m : this->sphereMeshes) {
      m.intersects (ray, intersection);
    }
    return intersection.isIntersection ();
  }
};

DELEGATE_CONSTRUCTOR (Scene)
DELEGATE_DESTRUCTOR  (Scene)

DELEGATE1       (WingedMesh&      , Scene, newWingedMesh, MeshType)
DELEGATE2       (WingedMesh&      , Scene, newWingedMesh, MeshType, const Id&)
DELEGATE1       (void             , Scene, removeWingedMesh, const Id&)
DELEGATE1       (WingedMesh&      , Scene, wingedMesh, const Id&)
DELEGATE1_CONST (const WingedMesh&, Scene, wingedMesh, const Id&)
DELEGATE        (SphereMesh&      , Scene, newSphereMesh)
DELEGATE1       (SphereMesh&      , Scene, newSphereMesh, const Id&)
DELEGATE1       (void             , Scene, removeSphereMesh, const Id&)
DELEGATE1       (SphereMesh&      , Scene, sphereMesh, const Id&)
DELEGATE1_CONST (const SphereMesh&, Scene, sphereMesh, const Id&)
DELEGATE1       (void             , Scene, render, MeshType)
DELEGATE3       (bool             , Scene, intersects, MeshType, const Ray&, WingedFaceIntersection&)
DELEGATE2       (bool             , Scene, intersects, const Ray&, SphereNodeIntersection&)
