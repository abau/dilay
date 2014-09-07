#ifndef DILAY_SPHERE_NODE_INTERSECTION
#define DILAY_SPHERE_NODE_INTERSECTION

#include "macro.hpp"
#include "intersection.hpp"

class SphereMesh;
class SphereMeshNode;

class SphereNodeIntersection : public Intersection {
  public:
    DECLARE_BIG6 (SphereNodeIntersection)

    SphereMesh&      mesh           () const;
    SphereMeshNode&  node           () const;
    void             update         ( float, const glm::vec3&, const glm::vec3&
                                    , SphereMesh&, SphereMeshNode& );

  private:
    IMPLEMENTATION
};

#endif
