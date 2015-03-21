#ifndef DILAY_ACTION_NEW_WINGED_MESH
#define DILAY_ACTION_NEW_WINGED_MESH

class MeshDefinition;
class Scene;
class WingedMesh;

namespace Action {

  WingedMesh& newWingedMesh (Scene&, const MeshDefinition&);
};

#endif
