/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SCENE
#define DILAY_SCENE

#include <string>
#include "configurable.hpp"
#include "macro.hpp"
#include "sketch/fwd.hpp"

class Camera;
class Mesh;
class PrimRay;
class RenderMode;
class WingedFaceIntersection;
class WingedMesh;

class Scene : public Configurable {
  public: 
    DECLARE_BIG3 (Scene, const Config&)

    WingedMesh&        newWingedMesh      (const Config&, const Mesh&);
    SketchMesh&        newSketchMesh      (const Config&, const SketchTree&);
    void               deleteMesh         (WingedMesh&);
    void               deleteMesh         (SketchMesh&);
    void               deleteWingedMeshes ();
    void               deleteSketchMeshes ();
    void               deleteEmptyMeshes  ();
    WingedMesh*        wingedMesh         (unsigned int);
    SketchMesh*        sketchMesh         (unsigned int);
    void               render             (Camera&);
    bool               intersects         (const PrimRay&, WingedFaceIntersection&);
    bool               intersects         (const PrimRay&, SketchTreeIntersection&);
    void               printStatistics    (bool) const;
    void               forEachMesh        (const std::function <void (WingedMesh&)>&);
    void               forEachMesh        (const std::function <void (SketchMesh&)>&);
    void               forEachConstMesh   (const std::function <void (const WingedMesh&)>&) const;
    void               forEachConstMesh   (const std::function <void (const SketchMesh&)>&) const;
    void               reset              ();
    const RenderMode&  commonRenderMode   () const;
    void               commonRenderMode   (const RenderMode&);
    bool               isEmpty            () const;
    unsigned int       numWingedMeshes    () const;
    unsigned int       numSketchMeshes    () const;
    unsigned int       numFaces           () const;
    bool               hasFileName        () const;
    const std::string& fileName           () const;
    void               fileName           (const std::string&);
    bool               toObjFile          ();
    bool               toObjFile          (const std::string&);
    bool               fromObjFile        (const Config&, const std::string&);

    SAFE_REF1 (WingedMesh, wingedMesh, unsigned int)
    SAFE_REF1 (SketchMesh, sketchMesh, unsigned int)

  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif
