/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SCENE
#define DILAY_SCENE

#include <string>
#include "configurable.hpp"
#include "macro.hpp"
#include "sketch/fwd.hpp"

class Camera;
class DynamicMesh;
class DynamicMeshIntersection;
class Intersection;
class Mesh;
class PrimRay;
class RenderMode;

class Scene : public Configurable {
  public: 
    DECLARE_BIG3 (Scene, const Config&)

    DynamicMesh&       newDynamicMesh      (const Config&, const DynamicMesh&);
    DynamicMesh&       newDynamicMesh      (const Config&, const Mesh&);
    SketchMesh&        newSketchMesh       (const Config&, const SketchMesh&);
    SketchMesh&        newSketchMesh       (const Config&, const SketchTree&);
    void               deleteMesh          (DynamicMesh&);
    void               deleteMesh          (SketchMesh&);
    void               deleteDynamicMeshes ();
    void               deleteSketchMeshes  ();
    void               deleteEmptyMeshes   ();
    void               render              (Camera&);
    bool               intersects          (const PrimRay&, DynamicMeshIntersection&);
    bool               intersects          (const PrimRay&, SketchNodeIntersection&);
    bool               intersects          (const PrimRay&, SketchBoneIntersection&);
    bool               intersects          (const PrimRay&, SketchMeshIntersection&);
    bool               intersects          (const PrimRay&, SketchMeshIntersection&, unsigned int);
    bool               intersects          (const PrimRay&, SketchPathIntersection&);
    bool               intersects          (const PrimRay&, Intersection&);
    void               printStatistics     () const;
    void               forEachMesh         (const std::function <void (DynamicMesh&)>&);
    void               forEachMesh         (const std::function <void (SketchMesh&)>&);
    void               forEachConstMesh    (const std::function <void (const DynamicMesh&)>&) const;
    void               forEachConstMesh    (const std::function <void (const SketchMesh&)>&) const;
    void               sanitizeMeshes      ();
    void               reset               ();
    bool               renderWireframe     () const;
    void               renderWireframe     (bool);
    void               toggleWireframe     ();
    void               toggleShading       ();
    bool               isEmpty             () const;
    unsigned int       numDynamicMeshes    () const;
    unsigned int       numSketchMeshes     () const;
    unsigned int       numFaces            () const;
    bool               hasFileName         () const;
    const std::string& fileName            () const;
    bool               toDlyFile           (bool);
    bool               toDlyFile           (const std::string&, bool);
    bool               fromDlyFile         (const Config&, const std::string&);

  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif
