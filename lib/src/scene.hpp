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
class Intersection;
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
    bool               intersects         (const PrimRay&, SketchNodeIntersection&);
    bool               intersects         (const PrimRay&, SketchBoneIntersection&);
    bool               intersects         (const PrimRay&, SketchMeshIntersection&);
    bool               intersects         (const PrimRay&, SketchMeshIntersection&, unsigned int);
    bool               intersects         (const PrimRay&, SketchPathIntersection&);
    bool               intersects         (const PrimRay&, Intersection&);
    void               printStatistics    (bool) const;
    void               forEachMesh        (const std::function <void (WingedMesh&)>&);
    void               forEachMesh        (const std::function <void (SketchMesh&)>&);
    void               forEachConstMesh   (const std::function <void (const WingedMesh&)>&) const;
    void               forEachConstMesh   (const std::function <void (const SketchMesh&)>&) const;
    void               sanitizeMeshes     ();
    void               reset              ();
    const RenderMode&  commonRenderMode   () const;
    void               commonRenderMode   (const RenderMode&);
    void               renderWireframe    (bool);
    void               toggleWireframe    ();
    void               toggleShading      ();
    bool               isEmpty            () const;
    unsigned int       numWingedMeshes    () const;
    unsigned int       numSketchMeshes    () const;
    unsigned int       numFaces           () const;
    bool               hasFileName        () const;
    const std::string& fileName           () const;
    bool               toDlyFile          (bool);
    bool               toDlyFile          (const std::string&, bool);
    bool               fromDlyFile        (const Config&, const std::string&);

    SAFE_REF1 (WingedMesh, wingedMesh, unsigned int)
    SAFE_REF1 (SketchMesh, sketchMesh, unsigned int)

  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif
