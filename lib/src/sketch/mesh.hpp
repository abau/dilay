/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
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

class SketchSphere {
  public:
    SketchSphere (unsigned int i, const glm::vec3& p, float r)
      : _index  (i)
      , _sphere (p, r)
    {}

    unsigned int      index  () const { return this->_index;            }
    const PrimSphere& sphere () const { return this->_sphere;           }
    const glm::vec3&  center () const { return this->_sphere.center (); }
    float             radius () const { return this->_sphere.radius (); }

    void center (const glm::vec3& p) { this->_sphere.center (p); }
    void radius (float r)            { this->_sphere.radius (r); }

  private:
    unsigned int _index;
    PrimSphere   _sphere;
};

class SketchMesh : public Configurable, public IntrusiveList <SketchMesh>::Item {
  public: 
    DECLARE_BIG3 (SketchMesh, unsigned int);

    bool operator== (const SketchMesh&) const;
    bool operator!= (const SketchMesh&) const;

    unsigned int         index           () const;
    const SketchTree&    tree            () const;
    void                 fromTree        (const SketchTree&);
    void                 reset           ();
    bool                 intersects      (const PrimRay&, SketchNodeIntersection&);
    bool                 intersects      (const PrimRay&, SketchBoneIntersection&);
    bool                 intersects      (const PrimRay&, SketchMeshIntersection&);
    bool                 intersects      (const PrimRay&, SketchMeshIntersection&, unsigned int);
    void                 render          (Camera&);
    void                 renderWireframe (bool);
    PrimPlane            mirrorPlane     (Dimension);
    SketchNode&          addChild        (SketchNode&, const glm::vec3&, float, const Dimension*);
    SketchNode&          addParent       (SketchNode&, const glm::vec3&, float, const Dimension*);
    void                 addSphere       (const glm::vec3&, float, const Dimension*);
    void                 move            (SketchNode&, const glm::vec3&, bool, const Dimension*);
    void                 scale           (SketchNode&, float, bool, const Dimension*);
    void                 deleteNode      (SketchNode&, bool, const Dimension*);
    void                 mirror          (Dimension);
    void                 rebalance       (SketchNode&);
    SketchNode&          snap            (SketchNode&, Dimension);
    void                 scaleSpheres    (const glm::vec3&, float, float, const Dimension*);
    void                 minMax          (glm::vec3&, glm::vec3&) const;
    const SketchSpheres& spheres         () const;

  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif
