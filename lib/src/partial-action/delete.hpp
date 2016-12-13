/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PARTIAL_ACTION_DELETE
#define DILAY_PARTIAL_ACTION_DELETE

class AffectedFaces;
class WingedEdge;
class WingedFace;
class WingedMesh;
class WingedVertex;

namespace PartialAction {
  void deleteEdgeAndRightFace (WingedMesh&, WingedEdge&, AffectedFaces&);
  WingedFace& deleteVertex (WingedMesh&, WingedVertex&, AffectedFaces&);
  void deleteValence3Vertex (WingedMesh&, WingedVertex&, AffectedFaces&);
}

#endif
