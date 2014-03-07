#ifndef DILAY_SPHERE_NODE_INTERSECTION
#define DILAY_SPHERE_NODE_INTERSECTION

#include "macro.hpp"

class SphereMesh;
class SphereMeshNode;

class SphereNodeIntersection {
  public:
    DECLARE_BIG6 (SphereNodeIntersection)

    bool             isIntersection () const;
    float            distance       () const;
    const glm::vec3& position       () const;
    SphereMesh&      mesh           () const;
    SphereMeshNode&  node           () const;
    void             update         (float, const glm::vec3&, SphereMesh&, SphereMeshNode&);
    void             reset          ();

  private:
    class Impl;
    Impl* impl;
};

#endif
