#ifndef DILAY_PARTIAL_ACTION_SMOOTH
#define DILAY_PARTIAL_ACTION_SMOOTH

#include "winged/fwd.hpp"

class AffectedFaces;

namespace PartialAction {

  void smooth (WingedMesh&, const VertexPtrSet&, unsigned int, AffectedFaces&);
};

#endif
