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
class PrimRay;

class SketchNodeData {
  public:
    SketchNodeData (const glm::vec3& pos, float r)
      : _position (pos)
      , _radius   (r)
      , _parent   (nullptr)
      , _mirrored (nullptr)
    {}

    const glm::vec3& position () const { return this->_position; }
    float            radius   () const { return this->_radius;   }
    SketchNode*      parent   () const { return this->_parent;   }
    SketchNode*      mirrored () const { return this->_mirrored; }

    void             position (const glm::vec3& p) { this->_position = p; }
    void             radius   (float r)            { this->_radius   = r; }
    void             parent   (SketchNode* p)      { this->_parent   = p; }
    void             mirrored (SketchNode* m)      { this->_mirrored = m; }

    void             updatePointers (const SketchTree::PtrMap&);

  private:
    glm::vec3   _position;
    float       _radius;
    SketchNode* _parent;
    SketchNode* _mirrored;
};

class SketchMesh : public Configurable, public IntrusiveList <SketchMesh>::Item {
  public: 
    DECLARE_BIG3 (SketchMesh, unsigned int);

    bool operator== (const SketchMesh&) const;
    bool operator!= (const SketchMesh&) const;

    unsigned int      index           () const;
    SketchTree&       tree            ();
    const SketchTree& tree            () const;
    void              fromTree        (const SketchTree&);
    void              reset           ();
    bool              intersects      (const PrimRay&, SketchNodeIntersection&);
    void              render          (Camera&);
    void              renderWireframe (bool);
    SketchNode&       addChild        (SketchNode&, const glm::vec3&, float, const Dimension*);
    void              move            (SketchNode&, const glm::vec3&, bool, const Dimension*);
    void              mirror          (Dimension);

  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif
