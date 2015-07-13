/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SCENE
#define DILAY_SCENE

#include <string>
#include "configurable.hpp"
#include "macro.hpp"

class Camera;
class Mesh;
class PrimRay;
class RenderMode;
class WingedFaceIntersection;
class WingedMesh;

class Scene : public Configurable {
  public: 
    DECLARE_BIG3 (Scene, const Config&)

    WingedMesh&        newWingedMesh     ();
    WingedMesh&        newWingedMesh     (const Mesh&);
    void               deleteMesh        (WingedMesh&);
    WingedMesh*        wingedMesh        (unsigned int) const;
    void               render            (Camera&);
    bool               intersects        (const PrimRay&, WingedFaceIntersection&);
    void               printStatistics   (bool) const;
    void               forEachMesh       (const std::function <void (WingedMesh&)>&);
    void               forEachConstMesh  (const std::function <void (const WingedMesh&)>&) const;
    void               reset             ();
    const RenderMode&  commonRenderMode  () const;
    void               commonRenderMode  (const RenderMode&);
    bool               isEmpty           () const;
    unsigned int       numWingedMeshes   () const;
    unsigned int       numFaces          () const;
    void               deleteEmptyMeshes ();
    bool               hasFileName       () const;
    const std::string& fileName          () const;
    void               fileName          (const std::string&);
    bool               toObjFile         ();
    bool               toObjFile         (const std::string&);
    bool               fromObjFile       (const std::string&);

    SAFE_REF1_CONST (WingedMesh, wingedMesh, unsigned int)
  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif
