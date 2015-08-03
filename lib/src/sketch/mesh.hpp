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
class PrimRay;

class SketchNodeData {
  public:
    SketchNodeData (const glm::vec3& p, float r)
      : _position (p)
      , _radius   (r)
    {}

    const glm::vec3& position   () const { return this->_position; }
    float            radius     () const { return this->_radius;   }

    void             position   (const glm::vec3& p) { this->_position = p; }
    void             radius     (float r)            { this->_radius   = r; }

  private:
    glm::vec3 _position;
    float     _radius;
};

class SketchMesh : public Configurable, public IntrusiveList <SketchMesh>::Item {
  public: 
    DECLARE_BIG3 (SketchMesh, unsigned int);

    bool operator== (const SketchMesh&) const;
    bool operator!= (const SketchMesh&) const;

    unsigned int      index           () const;
    bool              hasRoot         () const;
    SketchNode&       root            ();
    const SketchNode& root            () const;
    void              fromTree        (const SketchNode&);
    void              reset           ();
    bool              intersects      (const PrimRay&, SketchNodeIntersection&);
    void              render          (Camera&);
    void              renderWireframe (bool);

  private:
    IMPLEMENTATION

    void runFromConfig (const Config&);
};

#endif
