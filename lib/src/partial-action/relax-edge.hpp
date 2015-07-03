/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PARTIAL_ACTION_RELAX_EDGE
#define DILAY_PARTIAL_ACTION_RELAX_EDGE

class WingedMesh;
class WingedEdge;
class AffectedFaces;

namespace PartialAction {

  void relaxEdge (WingedMesh&, WingedEdge&, AffectedFaces&);
}

#endif
