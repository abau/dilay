#ifndef DILAY_PARTIAL_ACTION_VERTEX
#define DILAY_PARTIAL_ACTION_VERTEX

class AffectedFaces;
class WingedFace;
class WingedMesh;
class WingedVertex;

namespace PartialAction {

  WingedFace& collapseVertex (WingedMesh&, WingedVertex&, AffectedFaces&);
};

#endif
