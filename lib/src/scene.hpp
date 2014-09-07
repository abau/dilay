#ifndef DILAY_SCENE
#define DILAY_SCENE

#include <list>
#include "macro.hpp"

class WingedMesh;
class Id;
class PrimRay;
class WingedFaceIntersection;
enum class MeshType;
class SphereMesh;
class SphereNodeIntersection;
class IdSet;
enum class SelectionMode;
class Selection;
class Intersection;

typedef std::list <WingedMesh*> WingedMeshes;
typedef std::list <SphereMesh*> SphereMeshes;

class Scene {
  public: 
    DECLARE_BIG2 (Scene)

          WingedMesh&   newWingedMesh        (MeshType);
          WingedMesh&   newWingedMesh        (MeshType, const Id&);
          WingedMesh&   newWingedMesh        (MeshType, WingedMesh&&);
          WingedMesh&   wingedMesh           (const Id&);
    const WingedMesh&   wingedMesh           (const Id&) const;

          SphereMesh&   newSphereMesh        ();
          SphereMesh&   newSphereMesh        (const Id&);
          SphereMesh&   sphereMesh           (const Id&);
    const SphereMesh&   sphereMesh           (const Id&) const;

          void          deleteMesh           (MeshType, const Id&);
          void          render               (MeshType);
          bool          intersects           (SelectionMode, const PrimRay&, WingedFaceIntersection&);
          bool          intersects           (const PrimRay&, WingedFaceIntersection&);
          bool          intersects           (const PrimRay&, SphereNodeIntersection&);
          bool          intersects           (const PrimRay&, Intersection&);

          SelectionMode selectionMode        () const;
          void          changeSelectionMode  (SelectionMode);
    const Selection&    selection            () const;
          void          unselectAll          ();
          bool          selectIntersection   (const PrimRay&);
          unsigned int  numSelections        () const;

          WingedMeshes  selectedWingedMeshes (MeshType);
          SphereMeshes  selectedSphereMeshes ();

          void          printStatistics      () const;

  private:
    IMPLEMENTATION
};

#endif
