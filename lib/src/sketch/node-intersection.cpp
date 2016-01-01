/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "sketch/mesh.hpp"
#include "sketch/node-intersection.hpp"

struct SketchNodeIntersection::Impl {
  SketchNodeIntersection* self;
  SketchNode*            _node;

  Impl (SketchNodeIntersection* s) : self (s) {}

  bool update ( float d, const glm::vec3& p, const glm::vec3& n
              , SketchMesh& mesh, SketchNode& node)
  {
    if (this->self->SketchMeshIntersection::update (d,p,n,mesh)) {
      this->_node = &node;
      return true;
    }
    else {
      return false;
    }
  }

  SketchNode& node () const {
    assert (this->self->isIntersection ());
    assert (this->_node);
    return *this->_node;
  }
};

DELEGATE_BIG6_BASE (SketchNodeIntersection,(),(this),SketchMeshIntersection,())
DELEGATE5      (bool       , SketchNodeIntersection, update, float, const glm::vec3&, const glm::vec3&, SketchMesh&, SketchNode&)
DELEGATE_CONST (SketchNode&, SketchNodeIntersection, node)
