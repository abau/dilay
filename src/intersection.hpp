#ifndef DILAY_INTERSECTION
#define DILAY_INTERSECTION

#include "fwd-glm.hpp"
#include "fwd-winged.hpp"

class Sphere;

class FaceIntersectionImpl;

class FaceIntersection {
  public:
          FaceIntersection            ();
          FaceIntersection            (const glm::vec3&, LinkedFace);
          FaceIntersection            (const FaceIntersection&);
    const FaceIntersection& operator= (const FaceIntersection&);
         ~FaceIntersection            ();

    const glm::vec3& position () const;
    LinkedFace       face     () const;

    void             position (const glm::vec3&);
    void             face     (LinkedFace);

  private:
    FaceIntersectionImpl* impl;
};

namespace IntersectionUtil {
  bool intersects (const Sphere&, const WingedMesh&, const WingedVertex&);
  bool intersects (const Sphere&, const WingedMesh&, const WingedEdge&);
  bool intersects (const Sphere&, const WingedMesh&, const WingedFace&);
}

#endif
