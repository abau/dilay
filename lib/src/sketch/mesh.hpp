/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_SKETCH_MESH
#define DILAY_SKETCH_MESH

#include <glm/glm.hpp>
#include "configurable.hpp"
#include "intrusive-list.hpp"
#include "macro.hpp"
#include "sketch/fwd.hpp"

class Camera;
enum class Dimension;
class PrimPlane;
class PrimRay;
class PrimSphere;
enum class SketchPathSmoothEffect;

class SketchMesh : public Configurable, public IntrusiveList <SketchMesh>::Item {
  public: 
    DECLARE_BIG3 (SketchMesh, unsigned int);

    bool operator== (const SketchMesh&) const;
    bool operator!= (const SketchMesh&) const;

    unsigned int       index           () const;
    const SketchTree&  tree            () const;
    SketchTree&        tree            ();
    const SketchPaths& paths           () const;
    bool               isEmpty         () const;
    void               fromTree        (const SketchTree&);
    void               reset           ();
    bool               intersects      (const PrimRay&, SketchNodeIntersection&);
    bool               intersects      (const PrimRay&, SketchBoneIntersection&);
    bool               intersects      (const PrimRay&, SketchMeshIntersection&);
    bool               intersects      (const PrimRay&, SketchMeshIntersection&, unsigned int);
    bool               intersects      (const PrimRay&, SketchPathIntersection&);
    void               render          (Camera&);
    void               renderWireframe (bool);
    PrimPlane          mirrorPlane     (Dimension);
    SketchNode&        addChild        (SketchNode&, const glm::vec3&, float, const Dimension*);
    SketchNode&        addParent       (SketchNode&, const glm::vec3&, float, const Dimension*);
    SketchPath&        addPath         (const SketchPath&);
    void               addSphere       (bool, const glm::vec3&, const glm::vec3&, float, const Dimension*);
    void               move            (SketchNode&, const glm::vec3&, bool, const Dimension*);
    void               scale           (SketchNode&, float, bool, const Dimension*);
    void               deleteNode      (SketchNode&, bool, const Dimension*);
    void               deletePath      (SketchPath&, const Dimension*);
    void               mirror          (Dimension);
    void               rebalance       (SketchNode&);
    SketchNode&        snap            (SketchNode&, Dimension);
    void               minMax          (glm::vec3&, glm::vec3&) const;
    void               smoothPath      ( SketchPath&, const PrimSphere&, unsigned int
                                       , SketchPathSmoothEffect, const Dimension* );
    void               optimizePaths   ();

  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif
