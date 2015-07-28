/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "sketch/tree-intersection.hpp"

struct SketchTreeIntersection::Impl {
  SketchTreeIntersection* self;
  SketchMesh*            _mesh;
  SketchTree*            _tree;

  Impl (SketchTreeIntersection* s) : self (s) {}

  void update ( float d, const glm::vec3& p, const glm::vec3& n
              , SketchMesh& mesh, SketchTree& tree) 
  {
    if (this->self->Intersection::update (d,p,n)) {
      this->_mesh = &mesh;
      this->_tree = &tree;
    }
  }

  SketchMesh& mesh () const {
    assert (this->self->isIntersection ());
    assert (this->_mesh);
    return *this->_mesh;
  }

  SketchTree& tree () const {
    assert (this->self->isIntersection ());
    assert (this->_tree);
    return *this->_tree;
  }
};

DELEGATE_BIG6_BASE (SketchTreeIntersection,(),(this),Intersection,())
DELEGATE5      (void       , SketchTreeIntersection, update, float, const glm::vec3&, const glm::vec3&, SketchMesh&, SketchTree&)
DELEGATE_CONST (SketchMesh&, SketchTreeIntersection, mesh)
DELEGATE_CONST (SketchTree&, SketchTreeIntersection, tree)
