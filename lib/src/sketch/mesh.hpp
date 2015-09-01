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

class SketchNodeData {
  public:
    SketchNodeData ( const glm::vec3& pos, float r)
      : _position (pos)
      , _radius   (r)
    {}

    const glm::vec3& position () const { return this->_position; }
    float            radius   () const { return this->_radius;   }

    void position (const glm::vec3& p) { this->_position = p; }
    void radius   (float r)            { this->_radius   = r; }

  private:
    glm::vec3   _position;
    float       _radius;
};

class SketchMesh : public Configurable, public IntrusiveList <SketchMesh>::Item {
  public: 
    DECLARE_BIG3 (SketchMesh, unsigned int);

    bool operator== (const SketchMesh&) const;
    bool operator!= (const SketchMesh&) const;

    unsigned int      index           () const;
    const SketchTree& tree            () const;
    void              fromTree        (const SketchTree&);
    void              reset           ();
    bool              intersects      (const PrimRay&, SketchNodeIntersection&);
    bool              intersects      (const PrimRay&, SketchBoneIntersection&);
    void              render          (Camera&);
    void              renderWireframe (bool);
    PrimPlane         mirrorPlane     (Dimension);
    SketchNode&       addChild        (SketchNode&, const glm::vec3&, float, const Dimension*);
    SketchNode&       addParent       (SketchNode&, const glm::vec3&, float, const Dimension*);
    void              move            (SketchNode&, const glm::vec3&, bool, const Dimension*);
    void              scale           (SketchNode&, float, bool, const Dimension*);
    void              deleteNode      (SketchNode&, bool, const Dimension*);
    void              mirror          (Dimension);
    void              rebalance       (SketchNode&);
    SketchNode&       snap            (SketchNode&, Dimension);

  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif
