#include "scene.hpp"
#include "macro.hpp"
#include "id-map.hpp"
#include "winged/mesh.hpp"
#include "ray.hpp"
#include "mesh-type.hpp"
#include "sphere/mesh.hpp"

struct Scene :: Impl {
  IdMap <WingedMesh> wingedMeshes;
  IdMap <SphereMesh> sphereMeshes;

  WingedMesh& newWingedMesh (MeshType t) {
    assert (MeshType::FreeForm == t);
    return this->wingedMeshes.insert (new WingedMesh ());
  }

  WingedMesh& newWingedMesh (MeshType t, const Id& id) {
    assert (MeshType::FreeForm == t);
    return this->wingedMeshes.insert (new WingedMesh (id));
  }

  void removeWingedMesh (const Id& id) {
    assert (this->wingedMeshes.hasElement (id));
    this->wingedMeshes.erase (id);
  }

        WingedMesh& wingedMesh (const Id& id)       { return this->wingedMeshes.element (id); }
  const WingedMesh& wingedMesh (const Id& id) const { return this->wingedMeshes.element (id); }

  SphereMesh& newSphereMesh () {
    return this->sphereMeshes.insert (new SphereMesh ());
  }

  SphereMesh& newSphereMesh (const Id& id) {
    return this->sphereMeshes.insert (new SphereMesh (id));
  }

  void removeSphereMesh (const Id& id) {
    assert (this->sphereMeshes.hasElement (id));
    this->sphereMeshes.erase (id);
  }

        SphereMesh& sphereMesh (const Id& id)       { return this->sphereMeshes.element (id); }
  const SphereMesh& sphereMesh (const Id& id) const { return this->sphereMeshes.element (id); }

  void render (MeshType t) {
    if (t == MeshType::FreeForm) {
      for (auto it = this->wingedMeshes.begin (); it != this->wingedMeshes.end (); ++it) {
        it->second->render ();
      }
    }
    else if (t == MeshType::Sphere) {
      for (auto it = this->sphereMeshes.begin (); it != this->sphereMeshes.end (); ++it) {
        it->second->render ();
      }
    }
  }

  void intersectRay (MeshType t, const Ray& ray, WingedFaceIntersection& intersection) {
    if (t == MeshType::FreeForm) {
      for (auto it = this->wingedMeshes.begin (); it != this->wingedMeshes.end (); ++it) {
        it->second->intersectRay (ray, intersection);
      }
    }
  }
};

DELEGATE_CONSTRUCTOR (Scene)
DELEGATE_DESTRUCTOR  (Scene)

DELEGATE1       (WingedMesh&      , Scene, newWingedMesh, MeshType)
DELEGATE2       (WingedMesh&      , Scene, newWingedMesh, MeshType, const Id&)
DELEGATE1       (void             , Scene, removeWingedMesh, const Id&)
DELEGATE1       (WingedMesh&      , Scene, wingedMesh, const Id&)
DELEGATE1_CONST (const WingedMesh&, Scene, wingedMesh, const Id&)
DELEGATE1       (void             , Scene, render, MeshType)
DELEGATE3       (void             , Scene, intersectRay, MeshType, const Ray&, WingedFaceIntersection&)
