#include <glm/glm.hpp>
#include <utility>
#include "sphere/node-intersection.hpp"

struct SphereNodeIntersection::Impl {
  SphereNodeIntersection* self;
  SphereMesh*             _mesh;
  SphereMeshNode*         _node;

  Impl (SphereNodeIntersection* s) : self (s) {}

  void update ( float d, const glm::vec3& p, const glm::vec3& n
              , SphereMesh& mesh, SphereMeshNode& node) 
  {
    if (this->self->Intersection::update (d,p,n)) {
      this->_mesh = &mesh;
      this->_node = &node;
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
DELEGATE5      (void            , SphereNodeIntersection, update, float, const glm::vec3&, const glm::vec3&, SphereMesh&, SphereMeshNode&)
DELEGATE_CONST (SphereMesh&     , SphereNodeIntersection, mesh)
DELEGATE_CONST (SphereMeshNode& , SphereNodeIntersection, node)
