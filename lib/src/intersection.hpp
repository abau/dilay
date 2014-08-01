#ifndef DILAY_INTERSECTION
#define DILAY_INTERSECTION

#include <glm/fwd.hpp>
#include "macro.hpp"

class PrimSphere;
class PrimRay;
class WingedVertex;
class WingedEdge;
class WingedFace;
class WingedMesh;
class PrimPlane;
class PrimTriangle;
class PrimAABox;

class Intersection {
  public:
    DECLARE_BIG6_VIRTUAL (Intersection)

    void                 reset          ();
    bool                 isIntersection () const;
    const glm::vec3&     position       () const;
    float                distance       () const;
    bool                 update         (float, const glm::vec3&);

    static Intersection& min            (Intersection&, Intersection&);

  private:
    class Impl;
    Impl* impl;
};

namespace IntersectionUtil {
  bool intersects (const PrimSphere&, const glm::vec3&);
  bool intersects (const PrimSphere&, const WingedMesh&, const WingedVertex&);
  bool intersects (const PrimSphere&, const WingedMesh&, const WingedFace&);
  bool intersects (const PrimSphere&, const PrimAABox&);
  bool intersects (const PrimRay&, const PrimSphere&, float*); 
  bool intersects (const PrimRay&, const PrimPlane& , float*); 
  bool intersects (const PrimRay&, const PrimTriangle& , glm::vec3*); 
  bool intersects (const PrimRay&, const PrimAABox&); 
}

#endif
