/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PARTIAL_ACTION_TRIANGULATE_QUAD
#define DILAY_PARTIAL_ACTION_TRIANGULATE_QUAD

class WingedMesh;
class WingedFace;
class AffectedFaces;

namespace PartialAction {

  /* `triangulateQuad (m,f,n)` triangulates the quad `f` by inserting an edge from 
   * `f.edge ()->secondVertex (f)`.
   * `f` and the new face are added to `n`.
   */
  WingedEdge& triangulateQuad (WingedMesh&, WingedFace&, AffectedFaces&);
}
#endif
