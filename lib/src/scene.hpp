#ifndef DILAY_SCENE
#define DILAY_SCENE

#include "macro.hpp"

class Camera;
class ConfigProxy;
class PrimRay;
class WingedFaceIntersection;
class WingedMesh;

class Scene {
  public: 
    DECLARE_BIG3 (Scene, const ConfigProxy&)

    WingedMesh&       newWingedMesh    ();
    void              deleteMesh       (WingedMesh&);
    WingedMesh*       wingedMesh       (unsigned int) const;
    void              render           (Camera&);
    bool              intersects       (const PrimRay&, WingedFaceIntersection&);
    void              printStatistics  (bool) const;
    void              forEachMesh      (const std::function <void (WingedMesh&)>&);
    void              forEachConstMesh (const std::function <void (const WingedMesh&)>&) const;
    const WingedMesh* someWingedMesh   () const;

    SAFE_REF1_CONST (WingedMesh, wingedMesh, unsigned int)
  private:
    IMPLEMENTATION
};

#endif
