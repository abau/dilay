#ifndef DILAY_WINGED_UTIL
#define DILAY_WINGED_UTIL

class WingedMesh;
class WingedFace;
class WingedEdge;
class WingedVertex;

namespace WingedUtil {
  /** `printStatistics (w,b)` prints some statistics about `w`.
   * If `b == false`, only direct attributes are printed, i.e. no derived attributes
   * (e.g. normals) are evaluated and printed.
   */
  void printStatistics (const WingedMesh&  , const WingedVertex&, bool = true);
  void printStatistics (                     const WingedEdge&);
  void printStatistics (const WingedMesh&  , const WingedFace&  , bool = true);
  void printStatistics (const WingedMesh&  , bool = true);
}

#endif
