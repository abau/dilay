#ifndef DILAY_PARTIAL_ACTION_COLLAPSE_VALENCE_3_VERTEX
#define DILAY_PARTIAL_ACTION_COLLAPSE_VALENCE_3_VERTEX

class AffectedFaces;
class WingedMesh;
class WingedVertex;

namespace PartialAction {

  void collapseValence3Vertex (WingedMesh&, WingedVertex&, bool, AffectedFaces&);
};

#endif
