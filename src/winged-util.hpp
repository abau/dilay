#ifndef DILAY_WINGED_UTIL
#define DILAY_WINGED_UTIL

#include <unordered_set>
#include <functional>
#include "fwd-winged.hpp"

namespace std {
  template <> struct hash <LinkedEdge> {
    size_t operator() (const LinkedEdge&) const;
  };
  template <> struct hash <LinkedFace> {
    size_t operator() (const LinkedFace&) const;
  };
}

class Mesh;

namespace WingedUtil {
  /** `printStatistics (w,b)` prints some statistics about `w`.
   * If `b == false`, only direct attributes are printed, i.e. no derived attributes
   * (e.g. normals) are evaluated and printed.
   */
  void printStatistics (const WingedMesh&  , const WingedVertex&, bool = true);
  void printStatistics (                     const WingedEdge&);
  void printStatistics (const WingedMesh&  , const WingedFace&  , bool = true);
  void printStatistics (const WingedMesh&  , bool = true);

  /** `fromMesh (w,m)` loads mesh `m` as winged mesh `w` */
  void fromMesh (WingedMesh&, const Mesh&);
}

#endif
