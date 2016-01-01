/* This file is part of Dilay
 * Copyright © 2015,2016 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PARTIAL_ACTION_COLLAPSE_EDGE
#define DILAY_PARTIAL_ACTION_COLLAPSE_EDGE

class AffectedFaces;
class WingedEdge;
class WingedMesh;

namespace PartialAction {

  bool collapseEdge (WingedMesh&, WingedEdge&, AffectedFaces&);
};

#endif
