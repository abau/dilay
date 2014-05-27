#include <utility>
#include <glm/glm.hpp>
#include "sphere/node-intersection.hpp"

struct SphereNodeIntersection::Impl {
  SphereNodeIntersection* self;
  SphereMesh*             _mesh;
  SphereMeshNode*         _node;

  Impl (SphereNodeIntersection* s) : self (s) {}

  void update (float d, const glm::vec3& p, SphereMesh& m, SphereMeshNode& n) {
    if (this->self->Intersection::update (d,p)) {
      this->_mesh = &m;
      this->_node = &n;
    }
  }

  SphereMesh& mesh () const {
    assert (this->self->isIntersection ());
    return *this->_mesh;
  }

  SphereMeshNode& node () const {
    assert (this->self->isIntersection ());
    return *this->_node;
  }
};

DELEGATE_BIG6_BASE (SphereNodeIntersection,(),(this),Intersection,())
DELEGATE4      (void            , SphereNodeIntersection, update, float, const glm::vec3&, SphereMesh&, SphereMeshNode&)
DELEGATE_CONST (SphereMesh&     , SphereNodeIntersection, mesh)
DELEGATE_CONST (SphereMeshNode& , SphereNodeIntersection, node)
