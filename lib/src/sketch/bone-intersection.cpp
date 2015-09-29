/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "sketch/mesh.hpp"
#include "sketch/bone-intersection.hpp"

struct SketchBoneIntersection::Impl {
  SketchBoneIntersection* self;
  SketchNode*            _child;
  glm::vec3              projectedPosition;

  Impl (SketchBoneIntersection* s) : self (s) {}

  bool update ( float d, const glm::vec3& p, const glm::vec3& projP
              , SketchMesh& mesh, SketchNode& child ) 
  {
    assert (child.parent ());

    if (this->self->SketchMeshIntersection::update (d, p, glm::vec3 (0.0f), mesh)) {
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
DELEGATE5      (bool            , SketchBoneIntersection, update, float, const glm::vec3&, const glm::vec3&, SketchMesh&, SketchNode&)
DELEGATE_CONST (SketchNode&     , SketchBoneIntersection, parent)
DELEGATE_CONST (SketchNode&     , SketchBoneIntersection, child)
GETTER_CONST   (const glm::vec3&, SketchBoneIntersection, projectedPosition)
