#ifndef DILAY_ACTION_SUBDIVIDE
#define DILAY_ACTION_SUBDIVIDE

#include <vector>
#include "action/on-post-processed-winged-mesh.hpp"
#include "macro.hpp"

class WingedFace;
class WingedMesh;
class Id;

class ActionSubdivide : public ActionOnPostProcessedWMesh {
  public: 
    DECLARE_BIG3 (ActionSubdivide)

    // Subdivides a mesh's face and returns the IDs of all
    // affected faces (may contain dublicates).
    void run (WingedMesh&, WingedFace&, std::vector <Id>* = nullptr);

  private:
    void runUndoBeforePostProcessing (WingedMesh&);
    void runRedoBeforePostProcessing (WingedMesh&);

    class Impl;
    Impl* impl;
};

#endif
