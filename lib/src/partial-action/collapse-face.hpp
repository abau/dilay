#ifndef DILAY_PARTIAL_ACTION_COLLAPSE_FACE
#define DILAY_PARTIAL_ACTION_COLLAPSE_FACE

class AffectedFaces;
class WingedFace;
class WingedMesh;

namespace PartialAction {

  void collapseFace (WingedMesh&, WingedFace&, AffectedFaces&);
};

#endif
