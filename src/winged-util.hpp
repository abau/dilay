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
  /** `printStatistics (w,b)` prints some statistics of winged mesh `w`.
   * If `b == false`, only direct attributes are printed, i.e. no derived attributes
   * (e.g. normals) are evaluated and printed.
   */
  void printStatistics (const WingedMesh&, bool = true);

  /** `fromMesh (w,m)` loads mesh `m` as winged mesh `w` */
  void    fromMesh             (WingedMesh&, const Mesh&);

  /** Returns a set of edges from a set of faces, where each returned edge
   * is adjacent to at least one of the passed faces. */
  EdgeSet edgesFromFaces (const FaceSet&);
}

#endif
