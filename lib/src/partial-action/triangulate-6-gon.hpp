/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PARTIAL_ACTION_TRIANGULATE_6_GON
#define DILAY_PARTIAL_ACTION_TRIANGULATE_6_GON

class WingedMesh;
class WingedFace;
class AffectedFaces;

namespace PartialAction {

  /** `triangulate6Gon (m,f,n)` triangulates the 6-gon `f`.
   * Note thate `f.edge ()->firstVertex (f)` must be an even vertex, i.e.
   * it must not be generated during the current subdivision step.
   * The new faces are adjacent to `f`.
   * `f` and the new faces are added to `n`.
   */
  void triangulate6Gon (WingedMesh&, WingedFace&, AffectedFaces&);
}
#endif
