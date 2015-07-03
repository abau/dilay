/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PARTIAL_ACTION_COLLAPSE_FACE
#define DILAY_PARTIAL_ACTION_COLLAPSE_FACE

class AffectedFaces;
class WingedFace;
class WingedMesh;

namespace PartialAction {

  void collapseFace (WingedMesh&, WingedFace&, AffectedFaces&);
};

#endif
