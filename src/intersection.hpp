#ifndef DILAY_INTERSECTION
#define DILAY_INTERSECTION

#include "fwd-glm.hpp"

class Sphere;
class Ray;
class OctreeNode;
class WingedVertex;
class WingedEdge;
class WingedFace;
class WingedMesh;

class FaceIntersection {
  public:
          FaceIntersection            ();
          FaceIntersection            (const FaceIntersection&);
    const FaceIntersection& operator= (const FaceIntersection&);
         ~FaceIntersection            ();

    bool             isIntersection () const;
    float            distance       () const;
    const glm::vec3& position       () const;
    WingedFace&      face           () const;
    void             update         (float, const glm::vec3&, WingedFace&);
    void             reset          ();

  private:
    class Impl;
    Impl* impl;
};

namespace IntersectionUtil {
  bool intersects (const Sphere&, const glm::vec3&);
  bool intersects (const Sphere&, const WingedMesh&, const WingedVertex&);
  bool intersects (const Sphere&, const WingedMesh&, const WingedEdge&);
  bool intersects (const Sphere&, const WingedMesh&, const WingedFace&);
  bool intersects (const Sphere&, const OctreeNode&);
  bool intersects (const Ray&, const Sphere&, float&); 
}

#endif
