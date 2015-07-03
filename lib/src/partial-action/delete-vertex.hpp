/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PARTIAL_ACTION_DELETE_VERTEX
#define DILAY_PARTIAL_ACTION_DELETE_VERTEX

class AffectedFaces;
class WingedFace;
class WingedMesh;
class WingedVertex;

namespace PartialAction {

  WingedFace& deleteVertex (WingedMesh&, WingedVertex&, AffectedFaces&);
};

#endif
