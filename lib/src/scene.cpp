#include <list>
#include <algorithm>
#include "scene.hpp"
#include "macro.hpp"
#include "id-map.hpp"
#include "winged/mesh.hpp"
#include "primitive/ray.hpp"
#include "mesh-type.hpp"
#include "sphere/mesh.hpp"
#include "winged/face-intersection.hpp"
#include "sphere/node-intersection.hpp"

struct Scene :: Impl {
  std::list <WingedMesh>  wingedMeshes;
  std::list <SphereMesh>  sphereMeshes;

  IdMapPtr <WingedMesh>   wingedMeshIdMap;
  IdMapPtr <SphereMesh>   sphereMeshIdMap;

  Selection               selection;

  WingedMesh& newWingedMesh (MeshType t) {
    return this->newWingedMesh (t, Id ());
  }

  WingedMesh& newWingedMesh (MeshType t, const Id& id) {
    assert (MeshType::Freeform == t);
    this->wingedMeshes.emplace_back (id);
    this->wingedMeshIdMap.insert (this->wingedMeshes.back ());
    return this->wingedMeshes.back ();
  }

  void deleteWingedMesh (const Id& id) {
    assert (this->wingedMeshIdMap.hasElement (id));
    this->wingedMeshIdMap.remove (id);
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

  void deleteSphereMesh (const Id& id) {
    assert (this->sphereMeshIdMap.hasElement (id));
    this->sphereMeshIdMap.remove (id);
    this->sphereMeshes.remove_if ([&id] (SphereMesh& m) { return m.id () == id; });
  }

        SphereMesh& sphereMesh (const Id& id)       { return this->sphereMeshIdMap.elementRef (id); }
  const SphereMesh& sphereMesh (const Id& id) const { return this->sphereMeshIdMap.elementRef (id); }

  void render (MeshType t) {
    if (t == MeshType::Freeform) {
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

  bool intersects (MeshType t, const PrimRay& ray, WingedFaceIntersection& intersection) {
    if (t == MeshType::Freeform) {
      for (WingedMesh& m : this->wingedMeshes) {
        m.intersects (ray, intersection);
      }
    }
    return intersection.isIntersection ();
  }

  bool intersects (const PrimRay& ray, SphereNodeIntersection& intersection) {
    for (SphereMesh& m : this->sphereMeshes) {
      m.intersects (ray, intersection);
    }
    return intersection.isIntersection ();
  }

  Id intersects (MeshType t, const PrimRay& ray) {
    if (t == MeshType::Freeform) {
      WingedFaceIntersection intersection;
      if (this->intersects (t, ray, intersection)) {
        return intersection.mesh ().id ();
      }
    }
    else if (t == MeshType::Sphere || t == MeshType::SphereNode) {
      SphereNodeIntersection intersection;
      if (this->intersects (ray, intersection)) {
        if (t == MeshType::Sphere) {
          return intersection.mesh ().id ();
        }
        else if (t == MeshType::SphereNode) {
          return intersection.node ().id ();
        }
        assert (false);
      }
    }
    return Id ();
  }

  bool unselectAll () {
    bool update = this->selection.size () > 0;
    this->selection.clear ();
    return update;
  }

  bool selectIntersection (MeshType t, const PrimRay& ray) {
    Id id = this->intersects (t, ray);
    if (id.isValid ()) {
      if (this->selection.count (id) > 0) {
        if (this->selection.size () > 1) {
          this->unselectAll ();
          this->selection.insert (id);
          return true;
        }
        else {
          return false;
        }
      }
      else {
        this->unselectAll ();
        this->selection.insert (id);
        return true;
      }
    }
    else {
      return this->unselectAll ();
    }
  }
};

DELEGATE_CONSTRUCTOR (Scene)
DELEGATE_DESTRUCTOR  (Scene)

DELEGATE1       (WingedMesh&      , Scene, newWingedMesh, MeshType)
DELEGATE2       (WingedMesh&      , Scene, newWingedMesh, MeshType, const Id&)
DELEGATE1       (void             , Scene, deleteWingedMesh, const Id&)
DELEGATE1       (WingedMesh&      , Scene, wingedMesh, const Id&)
DELEGATE1_CONST (const WingedMesh&, Scene, wingedMesh, const Id&)
DELEGATE        (SphereMesh&      , Scene, newSphereMesh)
DELEGATE1       (SphereMesh&      , Scene, newSphereMesh, const Id&)
DELEGATE1       (void             , Scene, deleteSphereMesh, const Id&)
DELEGATE1       (SphereMesh&      , Scene, sphereMesh, const Id&)
DELEGATE1_CONST (const SphereMesh&, Scene, sphereMesh, const Id&)
DELEGATE1       (void             , Scene, render, MeshType)
DELEGATE3       (bool             , Scene, intersects, MeshType, const PrimRay&, WingedFaceIntersection&)
DELEGATE2       (bool             , Scene, intersects, const PrimRay&, SphereNodeIntersection&)
DELEGATE2       (Id               , Scene, intersects, MeshType, const PrimRay&)
GETTER_CONST    (const Selection& , Scene, selection)
DELEGATE        (bool             , Scene, unselectAll)
DELEGATE2       (bool             , Scene, selectIntersection, MeshType, const PrimRay&)
