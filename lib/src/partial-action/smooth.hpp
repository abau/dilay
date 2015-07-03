/* This file is part of Dilay
 * Copyright © 2015 Alexander Bau
 * Use and redistribute under the terms of the GNU General Public License
 */
#ifndef DILAY_PARTIAL_ACTION_SMOOTH
#define DILAY_PARTIAL_ACTION_SMOOTH

#include "winged/fwd.hpp"

class AffectedFaces;

namespace PartialAction {

  void smooth (WingedMesh&, const VertexPtrSet&, unsigned int, AffectedFaces&);
};

#endif
