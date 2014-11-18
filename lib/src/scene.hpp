#ifndef DILAY_SCENE
#define DILAY_SCENE

#include "macro.hpp"

class WingedMesh;
class PrimRay;
class WingedFaceIntersection;
enum class MeshType;
enum class SelectionMode;
class Selection;
class Intersection;

class Scene {
  public: 
    DECLARE_BIG3 (Scene)

          WingedMesh&   newWingedMesh        (MeshType);
          void          deleteMesh           (WingedMesh&);
          WingedMesh*   wingedMesh           (unsigned int) const;

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

          void          printStatistics      () const;
          void          toggleRenderMode     (MeshType);
          void          forEachMesh          (const std::function <void (WingedMesh&)>&) const;
          void          forEachSelectedMesh  (const std::function <void (WingedMesh&)>&) const;

    SAFE_REF1_CONST (WingedMesh, wingedMesh, unsigned int)
  private:
    IMPLEMENTATION
};

#endif
