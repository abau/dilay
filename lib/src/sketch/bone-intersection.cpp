/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "sketch/mesh.hpp"
#include "sketch/bone-intersection.hpp"

struct SketchBoneIntersection::Impl {
  SketchBoneIntersection* self;
  SketchNode*            _child;
  glm::vec3              projectedPosition;

  Impl (SketchBoneIntersection* s) : self (s) {}

  bool update ( float d, const glm::vec3& p, const glm::vec3& projP, const glm::vec3& n
              , SketchMesh& mesh, SketchNode& child ) 
  {
    assert (child.parent ());

    if (this->self->SketchMeshIntersection::update (d, p, n, mesh)) {
      this->_child            = &child;
      this->projectedPosition = projP;
      return true;
    }
    else {
      return false;
    }
  }

  SketchNode& parent () const {
    assert (this->self->isIntersection ());
    assert (this->_child);
    return *this->_child->parent ();
  }

  SketchNode& child () const {
    assert (this->self->isIntersection ());
    assert (this->_child);
    return *this->_child;
  }
};

DELEGATE_BIG6_BASE (SketchBoneIntersection,(),(this),SketchMeshIntersection,())
DELEGATE_CONST (SketchNode&     , SketchBoneIntersection, parent)
DELEGATE_CONST (SketchNode&     , SketchBoneIntersection, child)
GETTER_CONST   (const glm::vec3&, SketchBoneIntersection, projectedPosition)

bool SketchBoneIntersection :: update ( float a1, const glm::vec3& a2
                                      , const glm::vec3& a3, const glm::vec3& a4
                                      , SketchMesh& a5, SketchNode& a6 )
{
  return this->impl->update (a1, a2, a3, a4, a5, a6);
}
