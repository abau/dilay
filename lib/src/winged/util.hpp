#ifndef DILAY_WINGED_UTIL
#define DILAY_WINGED_UTIL

class WingedMesh;
class WingedFace;
class WingedEdge;
class WingedVertex;
class Octree;

namespace WingedUtil {
  void printStatistics (const WingedMesh&, const WingedVertex&);
  void printStatistics (const WingedEdge&);
  void printStatistics (const WingedFace&);

  /** `printStatistics (m,b)` prints some statistics about mesh `m`.
   * If `b == true`, statistics about faces, edges, and vertices are printed as well.
   */
  void printStatistics (const WingedMesh&, bool);
  void printStatistics (const Octree&);
}

#endif
