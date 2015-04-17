#ifndef DILAY_PARTIAL_ACTION_COLLAPSE_VALENCE_2_VERTEX
#define DILAY_PARTIAL_ACTION_COLLAPSE_VALENCE_2_VERTEX

class AffectedFaces;
class WingedEdge;
class WingedMesh;
class WingedVertex;

namespace PartialAction {

  WingedEdge& collapseValence2Vertex (WingedMesh&, WingedVertex&, bool, AffectedFaces&);
};

#endif
