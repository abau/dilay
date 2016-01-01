/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PARTIAL_ACTION_DELETE_EDGE_FACE
#define DILAY_PARTIAL_ACTION_DELETE_EDGE_FACE

class AffectedFaces;
class WingedEdge;
class WingedMesh;

namespace PartialAction {

  /* `deleteEdgeFace (m,e,a)` deletes edge `e` and its _right_ face of mesh `m`. 
   * `e` and its right face are also deleted from `a`.
   */
  void deleteEdgeFace (WingedMesh&, WingedEdge&, AffectedFaces&);
}

#endif
