#include <algorithm>
#include <list>
#include "id-map.hpp"
#include "macro.hpp"
#include "mesh-type.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
#include "selection-mode.hpp"
#include "selection.hpp"
#include "winged/face-intersection.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/util.hpp"

struct Scene :: Impl {
  std::list <WingedMesh>  wingedMeshes;

  IdMapPtr <WingedMesh>   wingedMeshIdMap;

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

  WingedMesh& newWingedMesh (MeshType t, WingedMesh&& source) {
    assert (MeshType::Freeform == t);
    this->wingedMeshes.emplace_back (std::move (source));
    this->wingedMeshIdMap.insert (this->wingedMeshes.back ());
    return this->wingedMeshes.back ();
  }

        WingedMesh& wingedMesh (const Id& id)       { return this->wingedMeshIdMap.elementRef (id); }
  const WingedMesh& wingedMesh (const Id& id) const { return this->wingedMeshIdMap.elementRef (id); }

  void deleteMesh (MeshType t, const Id& id) {
    switch (t) {
      case MeshType::Freeform: {
        this->deleteMesh (this->wingedMeshIdMap.elementRef (id));
        break;
      }
    }
  }

  void deleteMesh (WingedMesh& mesh) {
    this->wingedMeshIdMap.remove (mesh.id ());
    this->wingedMeshes.remove_if ([&mesh] (WingedMesh& m) { return &m == &mesh; });
  }

  void render (MeshType t) {
    if (t == MeshType::Freeform) {
      for (WingedMesh& m : this->wingedMeshes) {
        m.render (this->selection);
      }
    }
    else {
      std::abort ();
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

  bool intersects (const PrimRay& ray, Intersection& intersection) {
    WingedFaceIntersection i1;
    this->intersects (ray, i1);

    return intersection.isIntersection ();
  }

  std::pair <Id,unsigned int> intersects (const PrimRay& ray) {
    switch (this->selectionMode) {
      case SelectionMode::Freeform: {
        WingedFaceIntersection intersection;
        if (this->intersects (ray, intersection)) {
          return std::pair <Id,unsigned int> ( intersection.mesh ().id    ()
                                             , intersection.face ().index () );
        }
        break;
      }
    }
    return std::pair <Id, unsigned int> ();
  }

  void unselectAll () {
    this->selection.reset ();
  }

  void changeSelectionMode (SelectionMode t) {
    switch (this->selectionMode) {
      default:
        this->unselectAll ();
        break;
    }
    this->selectionMode = t;
  }

  bool selectIntersection (const PrimRay& ray) {
    std::pair <Id,unsigned int> intersection = this->intersects (ray);
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

  
  WingedMeshes selectedWingedMeshes (MeshType t) {
    assert (t == MeshType::Freeform);
    assert (this->selectionMode == SelectionMode::Freeform);

    WingedMeshes meshes;
    this->selection.forEachMajor ([this,&meshes] (const Id& id) {
      meshes.push_back (&this->wingedMesh (id));
    });
    return meshes;
  }

  void printStatistics () const {
    for (const WingedMesh& m : this->wingedMeshes) {
      WingedUtil::printStatistics (m);
    }
  }

  void toggleRenderMode (MeshType t) {
    switch (t) {
      case MeshType::Freeform:
        for (WingedMesh& m : this->wingedMeshes) {
          m.toggleRenderMode ();
        }
        break;

      default:
        std::abort ();
    }
  }
};

DELEGATE_BIG3 (Scene)

DELEGATE1       (WingedMesh&      , Scene, newWingedMesh, MeshType)
DELEGATE2       (WingedMesh&      , Scene, newWingedMesh, MeshType, const Id&)
DELEGATE_BASE   (WingedMesh&      , Scene, newWingedMesh, (MeshType t, WingedMesh&& m), (t, std::move (m)))
DELEGATE1       (WingedMesh&      , Scene, wingedMesh, const Id&)
DELEGATE1_CONST (const WingedMesh&, Scene, wingedMesh, const Id&)
DELEGATE2       (void             , Scene, deleteMesh, MeshType, const Id&)
DELEGATE1       (void             , Scene, deleteMesh, WingedMesh&)
DELEGATE1       (void             , Scene, render, MeshType)
DELEGATE3       (bool             , Scene, intersects, SelectionMode, const PrimRay&, WingedFaceIntersection&)
DELEGATE2       (bool             , Scene, intersects, const PrimRay&, WingedFaceIntersection&)
DELEGATE2       (bool             , Scene, intersects, const PrimRay&, Intersection&)
GETTER_CONST    (SelectionMode    , Scene, selectionMode)
DELEGATE1       (void             , Scene, changeSelectionMode, SelectionMode)
GETTER_CONST    (const Selection& , Scene, selection)
DELEGATE        (void             , Scene, unselectAll)
DELEGATE1       (bool             , Scene, selectIntersection, const PrimRay&)
DELEGATE_CONST  (unsigned int     , Scene, numSelections)
DELEGATE1       (WingedMeshes     , Scene, selectedWingedMeshes, MeshType)
DELEGATE_CONST  (void             , Scene, printStatistics)
DELEGATE1       (void             , Scene, toggleRenderMode, MeshType)
