#ifndef DILAY_SCENE
#define DILAY_SCENE

#include "macro.hpp"

class Camera;
class ConfigProxy;
class Intersection;
class PrimRay;
class Selection;
enum class SelectionMode;
class WingedFaceIntersection;
class WingedMesh;

class Scene {
  public: 
    DECLARE_BIG3 (Scene, const ConfigProxy&)

          WingedMesh&   newWingedMesh        ();
          WingedMesh&   newWingedMesh        (unsigned int);
          void          deleteMesh           (WingedMesh&);
          WingedMesh*   wingedMesh           (unsigned int) const;

          bool          intersects           (SelectionMode, const PrimRay&, WingedFaceIntersection&);
          bool          intersects           (const PrimRay&, WingedFaceIntersection&);
          bool          intersects           (const PrimRay&, Intersection&);

          SelectionMode selectionMode        () const;
          void          changeSelectionMode  (SelectionMode);
    const Selection&    selection            () const;
          void          unselectAll          ();
          bool          selectIntersection   (const PrimRay&);
          unsigned int  numSelections        () const;

          void          printStatistics      (bool) const;
          void          forEachMesh          (const std::function <void (WingedMesh&)>&) const;
          void          forEachSelectedMesh  (const std::function <void (WingedMesh&)>&) const;

    template <typename T> T*   mesh                  (unsigned int) const;
    template <typename T> void render                (const Camera&);
    template <typename T> void toggleRenderMode      ();
    template <typename T> void toggleRenderWireframe ();

    SAFE_REF1_CONST (WingedMesh, wingedMesh, unsigned int)
  private:
    IMPLEMENTATION
};

#endif
