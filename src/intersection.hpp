#ifndef DILAY_INTERSECTION
#define DILAY_INTERSECTION

#include <glm/fwd.hpp>

class Sphere;
class Ray;
class OctreeNode;
class WingedVertex;
class WingedEdge;
class WingedFace;
class WingedMesh;
class Plane;

namespace IntersectionUtil {
  bool intersects (const Sphere&, const glm::vec3&);
  bool intersects (const Sphere&, const WingedMesh&, const WingedVertex&);
  bool intersects (const Sphere&, const WingedMesh&, const WingedEdge&);
  bool intersects (const Sphere&, const WingedMesh&, const WingedFace&);
  bool intersects (const Sphere&, const OctreeNode&);
  bool intersects (const Ray&, const Sphere&, float&); 
  bool intersects (const Ray&, const Plane& , float&); 
}

#endif
