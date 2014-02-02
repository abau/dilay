#include "scene.hpp"
#include "macro.hpp"
#include "id-map.hpp"
#include "winged/mesh.hpp"
#include "ray.hpp"

struct Scene :: Impl {
  IdMap <WingedMesh> wingedMeshes;

  WingedMesh& newWingedMesh () {
    return this->wingedMeshes.insert (new WingedMesh ());
  }

  WingedMesh& newWingedMesh (const Id& id) {
    return this->wingedMeshes.insert (new WingedMesh (id));
  }

  void removeWingedMesh (const Id& id) {
    this->wingedMeshes.erase (id);
  }

        WingedMesh& wingedMesh (const Id& id)       { return this->wingedMeshes.element (id); }
  const WingedMesh& wingedMesh (const Id& id) const { return this->wingedMeshes.element (id); }

  void render () {
    for (auto it = this->wingedMeshes.begin (); it != this->wingedMeshes.end (); ++it) {
      it->second->render ();
    }
  }

  void intersectRay (const Ray& ray, FaceIntersection& intersection) {
    for (auto it = this->wingedMeshes.begin (); it != this->wingedMeshes.end (); ++it) {
      it->second->intersectRay (ray, intersection);
    }
  }
};

DELEGATE_CONSTRUCTOR (Scene)
DELEGATE_DESTRUCTOR  (Scene)

DELEGATE        (WingedMesh&      , Scene, newWingedMesh)
DELEGATE1       (WingedMesh&      , Scene, newWingedMesh, const Id&)
DELEGATE1       (void             , Scene, removeWingedMesh, const Id&)
DELEGATE1       (WingedMesh&      , Scene, wingedMesh, const Id&)
DELEGATE1_CONST (const WingedMesh&, Scene, wingedMesh, const Id&)
DELEGATE        (void             , Scene, render)
DELEGATE2       (void             , Scene, intersectRay, const Ray&, FaceIntersection&)
