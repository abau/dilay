#ifndef DILAY_OCTREE_UTIL
#define DILAY_OCTREE_UTIL

class Octree;

namespace OctreeUtil {
  unsigned int numFaces        (const Octree&);
  void         printStatistics (const Octree&);
}

#endif
