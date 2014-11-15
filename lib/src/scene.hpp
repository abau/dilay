#ifndef DILAY_SCENE
#define DILAY_SCENE

#include <list>
#include "macro.hpp"

class WingedMesh;
class Id;
class PrimRay;
class WingedFaceIntersection;
enum class MeshType;
class IdSet;
enum class SelectionMode;
class Selection;
class Intersection;

typedef std::list <WingedMesh*> WingedMeshes;

class Scene {
  public: 
    DECLARE_BIG3 (Scene)

          WingedMesh&   newWingedMesh        (MeshType);
          WingedMesh&   newWingedMesh        (MeshType, const Id&);
          WingedMesh&   newWingedMesh        (MeshType, WingedMesh&&);
          WingedMesh&   wingedMesh           (const Id&);
    const WingedMesh&   wingedMesh           (const Id&) const;

          void          deleteMesh           (MeshType, const Id&);
          void          deleteMesh           (WingedMesh&);
          void          render               (MeshType);
          bool          intersects           (SelectionMode, const PrimRay&, WingedFaceIntersection&);
          bool          intersects           (const PrimRay&, WingedFaceIntersection&);
          bool          intersects           (const PrimRay&, Intersection&);

          SelectionMode selectionMode        () const;
          void          changeSelectionMode  (SelectionMode);
    const Selection&    selection            () const;
          void          unselectAll          ();
          bool          selectIntersection   (const PrimRay&);
          unsigned int  numSelections        () const;

          WingedMeshes  selectedWingedMeshes (MeshType);

          void          printStatistics      () const;
          void          toggleRenderMode     (MeshType);

  private:
    IMPLEMENTATION
};

#endif
