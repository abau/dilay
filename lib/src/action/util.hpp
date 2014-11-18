#ifndef DILAY_ACTION_UTIL
#define DILAY_ACTION_UTIL

class WingedMesh;
class WingedEdge;
class WingedFace;
class WingedVertex;
template <typename T> class Maybe;

namespace ActionUtil {

  Maybe <unsigned int> maybeIndex (const WingedEdge*);
  Maybe <unsigned int> maybeIndex (const WingedFace*);
  Maybe <unsigned int> maybeIndex (const WingedVertex*);

  WingedEdge*   wingedEdge   (const WingedMesh&, const Maybe <unsigned int>&);
  WingedFace*   wingedFace   (const WingedMesh&, const Maybe <unsigned int>&);
  WingedVertex* wingedVertex (const WingedMesh&, const Maybe <unsigned int>&);

};

#endif
