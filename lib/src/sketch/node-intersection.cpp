/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "sketch/mesh.hpp"
#include "sketch/node-intersection.hpp"

struct SketchNodeIntersection::Impl {
  SketchNodeIntersection* self;
  SketchMesh*            _mesh;
  SketchNode*            _node;

  Impl (SketchNodeIntersection* s) : self (s) {}

  void update ( float d, const glm::vec3& p, const glm::vec3& n
              , SketchMesh& mesh, SketchNode& node) 
  {
    if (this->self->Intersection::update (d,p,n)) {
      this->_mesh = &mesh;
      this->_node = &node;
    }
  }

  SketchMesh& mesh () const {
    assert (this->self->isIntersection ());
    assert (this->_mesh);
    return *this->_mesh;
  }

  SketchNode& node () const {
    assert (this->self->isIntersection ());
    assert (this->_node);
    return *this->_node;
  }
};

DELEGATE_BIG6_BASE (SketchNodeIntersection,(),(this),Intersection,())
DELEGATE5      (void       , SketchNodeIntersection, update, float, const glm::vec3&, const glm::vec3&, SketchMesh&, SketchNode&)
DELEGATE_CONST (SketchMesh&, SketchNodeIntersection, mesh)
DELEGATE_CONST (SketchNode&, SketchNodeIntersection, node)
