#ifndef DILAY_SCENE
#define DILAY_SCENE

#include "configurable.hpp"
#include "macro.hpp"

class Camera;
class Mesh;
class PrimRay;
class WingedFaceIntersection;
class WingedMesh;

class Scene : public Configurable {
  public: 
    DECLARE_BIG3 (Scene, const Config&)

    WingedMesh&       newWingedMesh    ();
    WingedMesh&       newWingedMesh    (const Mesh&);
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

    void runFromConfig (const Config&);
};

#endif
