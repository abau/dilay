#ifndef DILAY_PARTIAL_ACTION_COLLAPSE_FACE
#define DILAY_PARTIAL_ACTION_COLLAPSE_FACE

class WingedMesh;
class WingedFace;
class AffectedFaces;

namespace PartialAction {

  void collapseFace (WingedMesh&, WingedFace&, AffectedFaces&);
};

#endif
