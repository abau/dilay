#include <algorithm>
#include <list>
#include "config.hpp"
#include "indexable.hpp"
#include "macro.hpp"
#include "primitive/ray.hpp"
#include "scene.hpp"
#include "selection-mode.hpp"
#include "selection.hpp"
#include "winged/face-intersection.hpp"
#include "winged/face.hpp"
#include "winged/mesh.hpp"
#include "winged/util.hpp"

struct Scene :: Impl {
  const ConfigProxy          wingedMeshConfig;
  IndexableList <WingedMesh> wingedMeshes;
  Selection                  selection;
  SelectionMode              selectionMode;

  Impl (const ConfigProxy& wmConfig)
    : wingedMeshConfig (wmConfig)
    , selectionMode    (SelectionMode::WingedMesh)
  {}

  WingedMesh& newWingedMesh () {
    WingedMesh& mesh = this->wingedMeshes.emplace ();
    mesh.fromConfig (this->wingedMeshConfig);
    return mesh;
  }

  WingedMesh& newWingedMesh (unsigned int index) {
    WingedMesh& mesh = this->wingedMeshes.emplaceAt (index);
    mesh.fromConfig (this->wingedMeshConfig);
    return mesh;
  }

  void deleteMesh (WingedMesh& mesh) {
    return this->wingedMeshes.deleteElement (mesh);
  }

  WingedMesh* wingedMesh (unsigned int index) const { 
    return this->wingedMeshes.get (index); 
  }

  bool intersects (SelectionMode t, const PrimRay& ray, WingedFaceIntersection& intersection) {
    if (t == SelectionMode::WingedMesh) {
      this->forEachMesh ([this, &ray, &intersection] (WingedMesh& m) {
        m.intersects (ray, intersection);
      });
    }
    return intersection.isIntersection ();
  }

  bool intersects (const PrimRay& ray, WingedFaceIntersection& intersection) {
    return this->intersects (this->selectionMode, ray, intersection);
  }

  bool intersects (const PrimRay& ray, Intersection& intersection) {
    WingedFaceIntersection i1;
    this->intersects (ray, i1);

    intersection = i1;
    return intersection.isIntersection ();
  }

  bool intersects (const PrimRay& ray, std::pair <unsigned int, unsigned int>& result) {
    switch (this->selectionMode) {
      case SelectionMode::WingedMesh: {
        WingedFaceIntersection intersection;
        if (this->intersects (ray, intersection)) {
          result.first  = intersection.mesh ().index ();
          result.second = intersection.face ().index ();
          return true;
        }
        break;
      }
    }
    return false;
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
    std::pair <unsigned, unsigned int> intersection;
    if (this->intersects (ray, intersection)) {
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
  
  void printStatistics (bool printAll) const {
    this->forEachMesh ([printAll] (WingedMesh& m) {
      WingedUtil::printStatistics (m, printAll);
    });
  }

  void forEachMesh (const std::function <void (WingedMesh&)>& f) const {
    this->wingedMeshes.forEachElement (f);
  }

  void forEachSelectedMesh (const std::function <void (WingedMesh&)>& f) const {
    if (this->selectionMode == SelectionMode::WingedMesh) {
      this->selection.forEachMajor ([this,&f] (unsigned int index) {
        f (*this->wingedMesh (index));
      });
    }
  }
};

template <> 
WingedMesh* Scene::mesh <WingedMesh> (unsigned int index) const { 
  return this->wingedMesh (index); 
}

template <> 
void Scene::render <WingedMesh> (const Camera& camera) {
  this->forEachMesh ([this, &camera] (WingedMesh& m) {
    m.render (camera, this->selection ().hasMajor (m.index ()));
  });
}

template <> 
void Scene::toggleRenderMode <WingedMesh> () {
  this->forEachMesh ([] (WingedMesh& m) {
    m.toggleRenderMode ();
  });
}

template <> 
void Scene::toggleRenderWireframe <WingedMesh> () {
  this->forEachMesh ([] (WingedMesh& m) {
    m.toggleRenderWireframe ();
  });
}

DELEGATE1_BIG3 (Scene, const ConfigProxy&)

DELEGATE        (WingedMesh&      , Scene, newWingedMesh)
DELEGATE1       (WingedMesh&      , Scene, newWingedMesh, unsigned int)
DELEGATE1       (void             , Scene, deleteMesh, WingedMesh&)
DELEGATE1_CONST (WingedMesh*      , Scene, wingedMesh, unsigned int)
DELEGATE3       (bool             , Scene, intersects, SelectionMode, const PrimRay&, WingedFaceIntersection&)
DELEGATE2       (bool             , Scene, intersects, const PrimRay&, WingedFaceIntersection&)
DELEGATE2       (bool             , Scene, intersects, const PrimRay&, Intersection&)
GETTER_CONST    (SelectionMode    , Scene, selectionMode)
DELEGATE1       (void             , Scene, changeSelectionMode, SelectionMode)
GETTER_CONST    (const Selection& , Scene, selection)
DELEGATE        (void             , Scene, unselectAll)
DELEGATE1       (bool             , Scene, selectIntersection, const PrimRay&)
DELEGATE_CONST  (unsigned int     , Scene, numSelections)
DELEGATE1_CONST (void             , Scene, printStatistics, bool)
DELEGATE1_CONST (void             , Scene, forEachMesh, const std::function <void (WingedMesh&)>&)
DELEGATE1_CONST (void             , Scene, forEachSelectedMesh, const std::function <void (WingedMesh&)>&)
