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
