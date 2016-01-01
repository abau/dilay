/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "winged/face-intersection.hpp"

struct WingedFaceIntersection::Impl {
  WingedFaceIntersection* self;
  WingedMesh*            _mesh;
  WingedFace*            _face;

  Impl (WingedFaceIntersection* s) : self (s) {}

  void update ( float d, const glm::vec3& p, const glm::vec3& n
              , WingedMesh& mesh, WingedFace& face) 
  {
    if (this->self->Intersection::update (d,p,n)) {
      this->_mesh = &mesh;
      this->_face = &face;
    }
  }

  WingedMesh& mesh () const {
    assert (this->self->isIntersection ());
    assert (this->_mesh);
    return *this->_mesh;
  }

  WingedFace& face () const {
    assert (this->self->isIntersection ());
    assert (this->_face);
    return *this->_face;
  }
};

DELEGATE_BIG6_BASE (WingedFaceIntersection,(),(this),Intersection,())
DELEGATE5      (void       , WingedFaceIntersection, update, float, const glm::vec3&, const glm::vec3&, WingedMesh&, WingedFace&)
DELEGATE_CONST (WingedMesh&, WingedFaceIntersection, mesh)
DELEGATE_CONST (WingedFace&, WingedFaceIntersection, face)
