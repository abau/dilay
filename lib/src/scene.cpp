#include <list>
#include <algorithm>
#include "scene.hpp"
#include "macro.hpp"
#include "id-map.hpp"
#include "primitive/ray.hpp"
#include "mesh-type.hpp"
#include "sphere/mesh.hpp"
#include "winged/mesh.hpp"
#include "winged/face.hpp"
#include "winged/face-intersection.hpp"
#include "sphere/node-intersection.hpp"
#include "selection-mode.hpp"
#include "selection.hpp"

struct Scene :: Impl {
  std::list <WingedMesh>  wingedMeshes;
  std::list <SphereMesh>  sphereMeshes;

  IdMapPtr <WingedMesh>   wingedMeshIdMap;
  IdMapPtr <SphereMesh>   sphereMeshIdMap;

  Selection               selection;
  SelectionMode           selectionMode;

  WingedMesh& newWingedMesh (MeshType t) {
    return this->newWingedMesh (t, Id ());
  }

  WingedMesh& newWingedMesh (MeshType t, const Id& id) {
    assert (MeshType::Freeform == t);
    this->wingedMeshes.emplace_back (id);
    this->wingedMeshIdMap.insert (this->wingedMeshes.back ());
    return this->wingedMeshes.back ();
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

        SphereMesh& sphereMesh (const Id& id)       { return this->sphereMeshIdMap.elementRef (id); }
  const SphereMesh& sphereMesh (const Id& id) const { return this->sphereMeshIdMap.elementRef (id); }

  void deleteMesh (MeshType t, const Id& id) {
    switch (t) {
      case MeshType::Freeform:
        assert (this->wingedMeshIdMap.hasElement (id));
        this->wingedMeshIdMap.remove (id);
        this->wingedMeshes.remove_if ([&id] (WingedMesh& m) { return m.id () == id; });
        break;

      case MeshType::Sphere:
        assert (this->sphereMeshIdMap.hasElement (id));
        this->sphereMeshIdMap.remove (id);
        this->sphereMeshes.remove_if ([&id] (SphereMesh& m) { return m.id () == id; });
        break;
    }
  }
          
  void render (MeshType t) {
    if (t == MeshType::Freeform) {
      for (WingedMesh& m : this->wingedMeshes) {
        m.render (this->selection);
      }
    }
    else if (t == MeshType::Sphere) {
      for (SphereMesh& m : this->sphereMeshes) {
        m.render (this->selection);
      }
    }
  }

  bool intersects (SelectionMode t, const PrimRay& ray, WingedFaceIntersection& intersection) {
    if (t == SelectionMode::Freeform) {
      for (WingedMesh& m : this->wingedMeshes) {
        m.intersects (ray, intersection);
      }
    }
    return intersection.isIntersection ();
  }

  bool intersects (const PrimRay& ray, WingedFaceIntersection& intersection) {
    return this->intersects (this->selectionMode, ray, intersection);
  }

  bool intersects (const PrimRay& ray, SphereNodeIntersection& intersection) {
    for (SphereMesh& m : this->sphereMeshes) {
      m.intersects (ray, intersection);
    }
    return intersection.isIntersection ();
  }

  std::pair <Id,Id> intersects (const PrimRay& ray) {
    switch (this->selectionMode) {
      case SelectionMode::Freeform: {
        WingedFaceIntersection intersection;
        if (this->intersects (ray, intersection)) {
          return std::pair <Id,Id> ( intersection.mesh ().id ()
                                   , intersection.face ().id () );
        }
        break;
      }
      case SelectionMode::Sphere:
      case SelectionMode::SphereNode: {
        SphereNodeIntersection intersection;
        if (this->intersects (ray, intersection)) {
          return std::pair <Id,Id> ( intersection.mesh ().id ()
                                   , intersection.node ().id () );
        }
        break;
      }
    }
    return std::pair <Id, Id> ();
  }

  void unselectAll () {
    this->selection.reset ();
  }

  void changeSelectionMode (SelectionMode t) {
    switch (this->selectionMode) {
      case SelectionMode::Sphere: 
        if (t == SelectionMode::SphereNode) {
          const Selection oldSelection = this->selection;

          oldSelection.forEachMajor ([this] (const Id& id) {
            this->sphereMesh (id).root ().forEachNode (
              [&id,this] (SphereMeshNode& node) {
                this->selection.selectMinor (id, node.id ());
              }
            );
          });
        }
        else {
          this->unselectAll ();
        }
        break;

      case SelectionMode::SphereNode: 
        if (t == SelectionMode::Sphere) {
          this->selection.resetMinors ();
        }
        else {
          this->unselectAll ();
        }
        break;

      default:
        this->unselectAll ();
        break;
    }
    this->selectionMode = t;
  }

  bool selectIntersection (const PrimRay& ray) {
    std::pair <Id,Id> intersection = this->intersects (ray);
    if (intersection.first.isValid ()) {
      if (SelectionModeUtil::isMajor (this->selectionMode)) {
        this->selection.toggleMajor (intersection.first);
      }
      else {
        this->selection.toggleMinor (intersection.first, intersection.second);
      }
      return true;
    }
    return false;
  }

  unsigned int numSelections () const {
    return SelectionModeUtil::isMajor (this->selectionMode) 
            ? this->selection.numMajors ()
            : this->selection.numMinors ();
  }
};

DELEGATE_CONSTRUCTOR (Scene)
DELEGATE_DESTRUCTOR  (Scene)

DELEGATE1       (WingedMesh&      , Scene, newWingedMesh, MeshType)
DELEGATE2       (WingedMesh&      , Scene, newWingedMesh, MeshType, const Id&)
DELEGATE1       (WingedMesh&      , Scene, wingedMesh, const Id&)
DELEGATE1_CONST (const WingedMesh&, Scene, wingedMesh, const Id&)
DELEGATE        (SphereMesh&      , Scene, newSphereMesh)
DELEGATE1       (SphereMesh&      , Scene, newSphereMesh, const Id&)
DELEGATE1       (SphereMesh&      , Scene, sphereMesh, const Id&)
DELEGATE1_CONST (const SphereMesh&, Scene, sphereMesh, const Id&)
DELEGATE2       (void             , Scene, deleteMesh, MeshType, const Id&)
DELEGATE1       (void             , Scene, render, MeshType)
DELEGATE3       (bool             , Scene, intersects, SelectionMode, const PrimRay&, WingedFaceIntersection&)
DELEGATE2       (bool             , Scene, intersects, const PrimRay&, WingedFaceIntersection&)
DELEGATE2       (bool             , Scene, intersects, const PrimRay&, SphereNodeIntersection&)
GETTER_CONST    (SelectionMode    , Scene, selectionMode)
DELEGATE1       (void             , Scene, changeSelectionMode, SelectionMode)
GETTER_CONST    (const Selection& , Scene, selection)
DELEGATE        (void             , Scene, unselectAll)
DELEGATE1       (bool             , Scene, selectIntersection, const PrimRay&)
DELEGATE_CONST  (unsigned int     , Scene, numSelections)
