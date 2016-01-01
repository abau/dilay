/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_WINGED_UTIL
#define DILAY_WINGED_UTIL

#include <glm/fwd.hpp>
#include "winged/fwd.hpp"

namespace WingedUtil {
  void      printStatistics (const WingedMesh&, const WingedVertex&);
  void      printStatistics (const WingedEdge&);
  void      printStatistics (const WingedFace&);

  /** `printStatistics (m,b)` prints some statistics about mesh `m`.
   * If `b == true`, statistics about faces, edges, and vertices are printed as well. */
  void      printStatistics (const WingedMesh&, bool);

  glm::vec3 averageNormal   (const WingedMesh&, const VertexPtrSet&);
  glm::vec3 center          (const WingedMesh&, const VertexPtrSet&);
  glm::vec3 center          (const WingedMesh&, const WingedVertex&);
  float     averageLength   (const WingedMesh&, const EdgePtrVec&);
}

#endif
