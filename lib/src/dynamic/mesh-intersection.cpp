/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#include "dynamic/mesh.hpp"
#include "dynamic/mesh-intersection.hpp"
#include "util.hpp"

struct DynamicMeshIntersection::Impl {
  DynamicMeshIntersection* self;
  unsigned int             faceIndex;
  DynamicMesh*            _mesh;

  Impl (DynamicMeshIntersection* s)
    : self (s)
    , faceIndex (Util::invalidIndex ())
    , _mesh (nullptr)
  {}

  bool update (float d, const glm::vec3& p, const glm::vec3& n, unsigned int i, DynamicMesh& mesh) {
    if (this->self->Intersection::update (d, p, n)) {
      this->_mesh = &mesh;
      this->faceIndex = i;
      return true;
    }
    else {
      return false;
    }
  }

  DynamicMesh& mesh () const {
    assert (this->self->isIntersection ());
    assert (this->_mesh);
    return *this->_mesh;
  }
};

DELEGATE_BIG6_BASE (DynamicMeshIntersection,(),(this),Intersection,())
DELEGATE_CONST (DynamicMesh&, DynamicMeshIntersection, mesh)
GETTER_CONST (unsigned int, DynamicMeshIntersection, faceIndex)
DELEGATE5 (bool, DynamicMeshIntersection, update, float, const glm::vec3&, const glm::vec3&, unsigned int, DynamicMesh&)
