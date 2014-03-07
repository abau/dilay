#include <glm/glm.hpp>
#include "sphere/node-intersection.hpp"

struct SphereNodeIntersection::Impl {
  bool            isIntersection;
  float           distance;
  glm::vec3       position;
  SphereMesh*     _mesh;
  SphereMeshNode* _node;

  Impl () : isIntersection (false) {}

  void update (float d, const glm::vec3& p, SphereMesh& m, SphereMeshNode& n) {
    if (this->isIntersection == false || d < this->distance) {
      this->isIntersection = true;
      this->distance       = d;
      this->position       = p;
      this->_mesh          = &m;
      this->_node          = &n;
    }
  }

  void reset () {
    this->isIntersection = false;
  }

  SphereMesh& mesh () const {
    assert (this->isIntersection);
    return *this->_mesh;
  }

  SphereMeshNode& node () const {
    assert (this->isIntersection);
    return *this->_node;
  }
};

DELEGATE_BIG6  (SphereNodeIntersection)
DELEGATE4      (void            , SphereNodeIntersection, update, float, const glm::vec3&, SphereMesh&, SphereMeshNode&)
DELEGATE       (void            , SphereNodeIntersection, reset)
DELEGATE_CONST (SphereMesh&     , SphereNodeIntersection, mesh)
DELEGATE_CONST (SphereMeshNode& , SphereNodeIntersection, node)
GETTER_CONST   (bool            , SphereNodeIntersection, isIntersection)
GETTER_CONST   (float           , SphereNodeIntersection, distance)
GETTER_CONST   (const glm::vec3&, SphereNodeIntersection, position)
